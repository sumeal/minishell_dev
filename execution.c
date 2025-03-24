/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 11:51:58 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/24 16:11:48 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execution(t_commands *commands, char ***mini_envp, int *g_exit_status)
{
	if (ft_strcmp(commands->cmd, "export") == 0)
	{	
		if (commands->next == NULL)
			export_variable(commands->args, mini_envp, g_exit_status);
		else
			execute_commands(commands->next, *mini_envp, g_exit_status);
	}
	else if (ft_strcmp(commands->cmd, "unset") == 0)
	{
		if (commands->next == NULL)
			unset_env(commands, *mini_envp, g_exit_status);
		else
			execute_commands(commands->next, *mini_envp, g_exit_status);
	}
	else if (ft_strcmp(commands->cmd, "exit") == 0)
	{
		ft_putstr_fd("exit\n", STDERR_FILENO);
		if (commands->args[1])
			check_exit_value(commands, g_exit_status);
		exit_program(commands, *mini_envp, g_exit_status);
	}
	else
		execute_commands(commands, *mini_envp, g_exit_status);
}

void	execute_commands(t_commands *cmd_list, char **envp, int *g_exit_status)
{
	t_exec_cmd	vars;

	save_original_fd(&vars);
	if (setup_input(cmd_list, &vars) < 0)
		return ;
	while (cmd_list)
	{
		dup2(vars.fdin, STDIN_FILENO);
		close(vars.fdin);
		if (setup_output(cmd_list, &vars) < 0)
			return ;
		dup2(vars.fdout, STDOUT_FILENO);
		close(vars.fdout);
		vars.builtin_executed
			= execute_builtin_command(&cmd_list, &envp, g_exit_status);
		if (vars.builtin_executed == 0)
		{
			if (execute_external_command(cmd_list, &vars, envp, g_exit_status)
				< 0)
				break ;
		}
		cmd_list = cmd_list->next;
	}
	restore_original_fd(&vars);
}

int	execute_command(t_commands *cmd,
	t_exec_cmd *vars, char **envp, int *g_exit_status)
{
	char	*path;

	(void)g_exit_status;
	setup_signal_child();
	vars->pid = fork();
	if (vars->pid == 0)
	{
		path = get_path(cmd->cmd, envp);
		if (path == NULL)
		{
			printf("%s: command not found\n", cmd->cmd);
			exit(127);
		}
		execve(path, cmd->args, envp);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (vars->pid < 0)
		return (print_error("fork", -1));
	return (0);
}

int	execute_external_command(t_commands *cmd_list,
	t_exec_cmd *vars, char **envp, int *g_exit_status)
{
	if (execute_command(cmd_list, vars, envp, g_exit_status) < 0)
		return (-1);
	if (cmd_list->next == NULL)
	{
		waitpid(vars->pid, &vars->status, 0);
		if (vars->status)
			set_exit_status(vars, g_exit_status);
	}
	return (0);
}

int	execute_builtin_command(t_commands **cmd_list,
	char ***envp, int *g_exit_status)
{
	if (strcmp((*cmd_list)->cmd, "echo") == 0
		&& (*cmd_list)->args[1]
		&& strcmp((*cmd_list)->args[1], "$?") == 0)
		return (print_exit_status(g_exit_status));
	else if (strcmp((*cmd_list)->cmd, "cd") == 0)
	{
		change_directory(cmd_list, envp, g_exit_status);
		return (1);
	}
	else if (strcmp((*cmd_list)->cmd, "echo") == 0)
		return (print_echo((*cmd_list)->args, g_exit_status));
	else if (strcmp((*cmd_list)->cmd, "env") == 0)
		return (print_env(cmd_list, *envp, g_exit_status));
	else if (strcmp((*cmd_list)->cmd, "pwd") == 0)
		return (print_pwd(*envp, g_exit_status));
	return (0);
}
