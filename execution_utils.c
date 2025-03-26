/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 13:34:00 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/25 12:21:52 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	save_original_fd(t_exec_cmd *vars)
{
	vars->status = 0;
	vars->builtin_executed = -1;
	vars->ori_in = dup(STDIN_FILENO);
	vars->ori_out = dup(STDOUT_FILENO);
}

void	restore_original_fd(t_exec_cmd *vars)
{
	dup2(vars->ori_in, STDIN_FILENO);
	dup2(vars->ori_out, STDOUT_FILENO);
	close(vars->ori_in);
	close(vars->ori_out);
}

void	set_exit_status(t_exec_cmd *vars, int *g_exit_status)
{
	if (WIFEXITED(vars->status))
		*g_exit_status = WEXITSTATUS(vars->status);
	else if (WIFSIGNALED(vars->status))
		*g_exit_status = 128 + WTERMSIG(vars->status);
	else
		*g_exit_status = 127;
}

int	handle_piped_command_output(t_exec_cmd *vars)
{
	if (pipe(vars->pipefd) == -1)
		return (print_error("pipe", -1));
	vars->fdout = vars->pipefd[1];
	vars->fdin = vars->pipefd[0];
	return (0);
}

int	setup_output(t_cmd *cmd, t_exec_cmd *vars)
{
	if (cmd->next == NULL)
	{
		if (cmd->output_file)
		{
			if (handle_last_command_output(cmd, vars) < 0)
				return (-1);
		}
		else
			vars->fdout = dup(vars->ori_out);
	}
	else
		if (handle_piped_command_output(vars) < 0)
			return (-1);
	return (0);
}