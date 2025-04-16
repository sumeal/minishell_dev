/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 08:32:49 by abin-moh          #+#    #+#             */
/*   Updated: 2025/04/15 15:37:23 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_signal_heredoc(int signum)
{
	if (signum == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
		g_signal = 130;
	}
}

int	hd_printf(char *hd_delimeter)
{
	int		pipe_fd[2];
	char	*line;

	if (pipe(pipe_fd) < 0)
		return (-1);
	setup_signal_heredoc();
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal == 130)
		{
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			return (-1);
		}
		if (ft_strcmp(line, hd_delimeter) == 0)
		{
			free(line);
			break ;
		}
		ft_putendl_fd(line, pipe_fd[1]);
		free(line);
	}
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}

int	exit_function(t_cmd *commands, char **mini_envp, int *g_exit_status)
{
	if (!(commands->argv[1]))
	{
		printf("exit\n");
		exit_program(commands, mini_envp, g_exit_status);
	}
	else
		return (check_exit_value(commands, mini_envp, g_exit_status));
	return (1);
}

void	handle_input_redir(t_cmd *cmd, t_exec_cmd *vars, int *g_exit_status)
{
	if (vars->i == 0)
	{
		if (setup_input(cmd, vars, g_exit_status) < 0)
			exit(EXIT_FAILURE);
	}
	else if (vars->i > 0)
	{
		if (dup2(vars->pipefd[vars->i - 1][0], STDIN_FILENO) < 0)
		{
			perror("dup2 input");
			exit(EXIT_FAILURE);
		}
	}
}

void	handle_output_redir(t_cmd *cmd, int cmd_count, t_exec_cmd *vars)
{
	if (vars->i == cmd_count - 1)
	{
		if (cmd->output_file)
		{
			if (handle_last_command_output(cmd, vars) < 0)
				exit(EXIT_FAILURE);
		}
		else
			vars->fdout = dup(vars->ori_out);
	}
	else if (vars->i < cmd_count - 1)
	{
		if (dup2(vars->pipefd[vars->i][1], STDOUT_FILENO) < 0)
		{
			perror("dup2 output");
			exit(EXIT_FAILURE);
		}
	}
}
