/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 13:24:29 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/25 12:28:03 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	unset_env(t_cmd *commands, char **mini_envp, int *g_exit_status)
{
	int	i;

	i = 1;
	while (commands->argv[i])
	{
		unset_variable(commands->argv[i], &mini_envp);
		i++;
	}
	*g_exit_status = 0;
}

void	unset_variable(char *var, char ***envp)
{
	int	index;
	int	j;

	if (!var)
	{
		ft_putstr_fd("unset: missing argument\n", 2);
		return ;
	}
	index = find_index(var, *envp);
	if (index >= 0)
	{
		free((*envp)[index]);
		j = index;
		while ((*envp)[j])
		{
			(*envp)[j] = (*envp)[j + 1];
			j++;
		}
	}
}

int	find_index(char *var, char **envp)
{
	int		i;
	size_t	len;

	len = ft_strlen(var);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, len) == 0
			&& (envp[i][len] == '=' || envp[i][len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}
