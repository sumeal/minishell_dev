/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/07 15:18:47 by jchen2            #+#    #+#             */
/*   Updated: 2025/03/17 15:41:50 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	flag_func(char c, int *num, t_bool *flag)
{
	if ((c == '\'' || c == '\"') && !flag->s_s && !flag->d_s)
	{
		if (flag->w_s)
			flag->w_s = false;
		(*num)++;
		if (c == '\'')
			flag->s_s = !flag->s_s;
		else
			flag->d_s = !flag->d_s;
	}
	else if ((c == '\'' && flag->s_s) || (c == '\"' && flag->d_s))
	{
		if (flag->s_s)
			flag->s_s = !flag->s_s;
		else
			flag->d_s = !flag->d_s;
	}
	else if ((c != ' ' && c != '\n' && c != '\t')
		&& !flag->w_s && !flag->s_s && !flag->d_s)
	{
		(*num)++;
		flag->w_s = true;
	}
	else if ((c == ' ' || c == '\n' || c == '\t') && !flag->s_s && !flag->d_s)
		flag->w_s = false;
}

static int	num_substr(char *str)
{
	int		num;
	t_bool	flag;

	num = 0;
	flag.w_s = false;
	flag.s_s = false;
	flag.d_s = false;
	while (*str != '\0')
	{
		flag_func(*str, &num, &flag);
		str++;
	}
	return (num);
}

static char	**free_rest(char **arr, int i)
{
	i = i - 1;
	while (i >= 0)
	{
		free(arr[i]);
		i--;
	}
	free (arr);
	return (NULL);
}

static char	**set_substr(char *str, int num)
{
	char	**arr;
	int		i;
	t_bool	flag;

	arr = (char **)malloc((num + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	flag.d_s = false;
	flag.s_s = false;
	flag.w_s = false;
	while (*str)
	{
		while (*str == ' ' || *str == '\n' || *str == '\t')
			str++;
		if (*str)
		{
			arr[i] = count_copy_substr(&str, &flag);
			if (!arr[i])
				return (free_rest(arr, i));
			i++;
		}
	}
	arr[i] = NULL;
	return (arr);
}

char	**ft_split_quotes(char *str)
{
	int		num;
	char	**arr;

	if (!str)
		return (NULL);
	num = num_substr(str);
	arr = set_substr(str, num);
	return (arr);
}
