/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 13:30:47 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/24 17:00:02 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exit_program(t_commands *commands, char **mini_envp, int *g_exit_status)
{
	free_commands(commands);
	free_path(mini_envp);
	exit (*g_exit_status);
}

int	is_num(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[0] == '-')
			i++ ;
		if (ft_isdigit(s[i]) == 0)
			return (0);
		i++;
	}
	return (1);
}

void	check_exit_value(t_commands *commands, int *g_exit_status)
{
	int	temp;

	if (is_num(commands->args[1]))
	{
		temp = ft_atoi(commands->args[1]);
		if (temp < 0)
			temp = temp + 256;
		*g_exit_status = (temp % 256);
	}
	else
	{
		ft_putstr_fd("bash: exit: ", STDERR_FILENO);
		ft_putstr_fd(commands->args[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		*g_exit_status = 2;
	}
}

void	free_commands(t_commands *commands)
{
	t_commands	*temp;
	int			i;

	while (commands)
	{
		temp = commands;
		commands = commands->next;
		if (temp->cmd)
			free(temp->cmd);
		if (temp->args)
		{
			i = -1;
			while (++i < temp->argc)
				free(temp->args[i]);
			free(temp->args);
		}
		if (temp->input_file)
			free(temp->input_file);
		if (temp->output_file)
			free(temp->output_file);
		if (temp->delimiter)
			free(temp->delimiter);
		free(temp);
	}
}
