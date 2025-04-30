/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/24 13:06:11 by abin-moh          #+#    #+#             */
/*   Updated: 2025/04/29 14:40:37 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_envp(char **envp)
{
	int	count;

	count = 0;
	while (envp[count])
		count++;
	return (count);
}

char	*get_var_name(char *envp)
{
	char	*pos;

	pos = ft_strchr(envp, '=');
	if (!pos)
		return (ft_strdup(envp));
	return (ft_substr(envp, 0, pos - envp));
}

int	add_variable_to_env(char ***mini_envp, char **args)
{
	int		i;
	int		index;
	char	*new;

	i = 1;
	while (args[i])
	{
		if (check_valid_value(args[i]) < 0)
			return (0);
		index = find_variable(args[i], *mini_envp);
		new = ft_strdup(args[i]);
		if (!new)
			return (0);
		if (index >= 0)
		{
			free((*mini_envp)[index]);
			(*mini_envp)[index] = new;
		}
		else
			add_new_variable(mini_envp, &new);
		i++;
	}
	return (1);
}

int	find_variable(char *args, char **mini_envp)
{
	int		i;
	char	*name1;
	char	*name2;

	name1 = get_var_name(args);
	i = 0;
	while (mini_envp[i])
	{
		name2 = get_var_name(mini_envp[i]);
		if (name1 && name2 && ft_strcmp(name1, name2) == 0)
		{
			free(name1);
			free(name2);
			return (i);
		}
		free(name2);
		i++;
	}
	free(name1);
	return (-1);
}

int	check_valid_value(char *s)
{
	int	i;

	i = 0;
	if (s[0] == '=')
	{
		ft_putstr_fd("bash: export: `", STDERR_FILENO);
		ft_putstr_fd(s, STDERR_FILENO);
		ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
		return (-1);
	}
	while (s[i] && s[i] != '=')
	{
		if (!(ft_isalnum(s[i]) != 0 || s[i] == '_' || s[0] == '=')
			&& (ft_isdigit(s[0])))
		{
			ft_putstr_fd("bash: export: `", STDERR_FILENO);
			ft_putstr_fd(s, STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			return (-1);
		}
		i++;
	}
	return (0);
}
