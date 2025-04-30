/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/14 15:16:54 by abin-moh          #+#    #+#             */
/*   Updated: 2025/04/24 09:00:41 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	setup_io(t_cmd *cmd, int cmd_count,
			t_exec_cmd *vars, int *g_exit_status)
{
	handle_input_redir(cmd, vars, g_exit_status);
	handle_output_redir(cmd, cmd_count, vars);
	if (cmd_count != 1)
		closing_pipes(&vars);
}

void	execute_external_command(t_cmd *cur, char **envp)
{
	char	*path;

	if (ft_strncmp(cur->argv[0], "./", 2) == 0)
		execve(cur->argv[0], cur->argv, envp);
	if (ft_strncmp(cur->argv[0], "/", 1) == 0)
		path = check_xcess(cur->argv[0]);
	else
		path = get_path(cur->argv[0], envp);
	if (path == NULL)
	{
		ft_putstr_fd(cur->argv[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(127);
	}
	execve(path, cur->argv, envp);
	perror("execve");
	exit(EXIT_FAILURE);
}

void	wait_for_child(t_exec_cmd *vars)
{
	int	i;

	i = 0;
	while (i < (*vars).cmd_count)
	{
		waitpid((*vars).pid[i], &vars->status, 0);
		i++;
	}
}

void	execute_command_in_child(t_cmd *cur, t_exec_cmd *vars,
			int *g_exit_status, char ***envp)
{
	setup_io(cur, vars->cmd_count, vars, g_exit_status);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (execute_builtin_command(&cur, envp, g_exit_status) == 1)
		exit(EXIT_SUCCESS);
	execute_external_command(cur, *envp);
	exit(EXIT_FAILURE);
}
