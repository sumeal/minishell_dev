/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 11:51:58 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/26 13:27:55 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execution(t_cmd *commands, char ***mini_envp, int *g_exit_status)
{
	if (ft_strcmp(commands->argv[0], "export") == 0)
	{	
		if (commands->next == NULL)
			export_variable(commands->argv, mini_envp, g_exit_status);
		else
			execute_commands(commands->next, *mini_envp, g_exit_status);
	}
	else if (ft_strcmp(commands->argv[0], "unset") == 0)
	{
		if (commands->next == NULL)
			unset_env(commands, *mini_envp, g_exit_status);
		else
			execute_commands(commands->next, *mini_envp, g_exit_status);
	}
	else if (ft_strcmp(commands->argv[0], "exit") == 0)
	{
		check_exit_value(commands, *mini_envp, g_exit_status);
		if (commands->next)
			execute_commands(commands->next, *mini_envp, g_exit_status);
	}
	else
		execute_commands(commands, *mini_envp, g_exit_status);
}

void	execute_commands(t_cmd *cmd_list, char **envp, int *g_exit_status)
{
	t_exec_cmd	vars;

	save_original_fd(&vars);
	if (setup_input(cmd_list, &vars, g_exit_status) < 0)
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

int	execute_command(t_cmd *cmd,
	t_exec_cmd *vars, char **envp, int *g_exit_status)
{
	char	*path;

	(void)g_exit_status;
	setup_signal_child();
	vars->pid = fork();
	if (vars->pid == 0)
	{
		path = get_path(cmd->argv[0], envp);
		if (path == NULL)
		{
			printf("%s: command not found\n", cmd->argv[0]);
			exit(127);
		}
		execve(path, cmd->argv, envp);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (vars->pid < 0)
		return (print_error("fork", -1));
	return (0);
}

int	execute_external_command(t_cmd *cmd_list,
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

int	execute_builtin_command(t_cmd **cmd,
	char ***envp, int *g_exit_status)
{
	if (ft_strcmp((*cmd)->argv[0], "echo") == 0
		&& (*cmd)->argv[1]
		&& ft_strcmp((*cmd)->argv[1], "$?") == 0)
		return (print_exit_status(g_exit_status));
	else if (ft_strcmp((*cmd)->argv[0], "cd") == 0 && (*cmd)->prev == NULL
		&& (*cmd)->next == NULL)
	{
		change_directory(cmd, envp, g_exit_status);
		return (1);
	}
	else if (ft_strcmp((*cmd)->argv[0], "echo") == 0)
		return (print_echo((*cmd)->argv, g_exit_status));
	else if (ft_strcmp((*cmd)->argv[0], "env") == 0)
		return (print_env(cmd, *envp, g_exit_status));
	else if (ft_strcmp((*cmd)->argv[0], "pwd") == 0)
		return (print_pwd(*envp, g_exit_status));
	else if (ft_strcmp((*cmd)->argv[0], "export") == 0
		|| ft_strcmp((*cmd)->argv[0], "unset") == 0
		|| ft_strcmp((*cmd)->argv[0], "cd") == 0)
		return (ignore_export(g_exit_status));
	return (0);
}
