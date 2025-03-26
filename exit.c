/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 13:30:47 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/25 12:22:49 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exit_program(t_cmd *commands, char **mini_envp, int *g_exit_status)
{
	free_cmds(commands);
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

void	check_exit_value(t_cmd *commands, char **mini_envp, int *g_exit_status)
{
	int	temp;

	if (commands->next != NULL)
		return ;
	ft_putstr_fd("exit\n", STDERR_FILENO);
	if (commands->argv[2] != NULL)
	{
		error_too_many_arg(g_exit_status);
		return ;
	}
	if (is_num(commands->argv[1]))
	{
		temp = ft_atoi(commands->argv[1]);
		if (temp < 0)
			temp = temp + 256;
		*g_exit_status = (temp % 256);
	}
	else
	{
		ft_putstr_fd("bash: exit: ", STDERR_FILENO);
		ft_putstr_fd(commands->argv[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		*g_exit_status = 2;
	}
	exit_program(commands, mini_envp, g_exit_status);
}

void	error_too_many_arg(int *g_exit_status)
{
	ft_putstr_fd("bash: exit: too many arguments\n", STDERR_FILENO);
	*g_exit_status = 1;
}
