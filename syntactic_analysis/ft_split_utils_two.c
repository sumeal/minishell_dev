/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_count.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 20:11:49 by jchen2            #+#    #+#             */
/*   Updated: 2025/03/13 20:13:56 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	count_single_quote(char **str, int *sublen, t_bool *flag)
{
	flag->s_s = true;
	(*sublen)++;
	(*str)++;
	while (**str && !(**str == '\'' && flag->s_s))
	{
		(*sublen)++;
		(*str)++;
	}
	if (**str == '\'')
	{
		(*sublen)++;
		(*str)++;
		flag->s_s = false;
	}
}

static void	count_double_quote(char **str, int *sublen, t_bool *flag)
{
	flag->d_s = true;
	(*sublen)++;
	(*str)++;
	while (**str && !(**str == '\"' && flag->d_s))
	{
		(*sublen)++;
		(*str)++;
	}
	if (**str == '\"')
	{
		(*sublen)++;
		(*str)++;
		flag->d_s = false;
	}
}

static void	count_word(char **str, int *sublen, t_bool *flag)
{
	flag->w_s = true;
	while (**str && **str != ' ' && **str != '\n' && **str != '\t'
		&& **str != '\'' && **str != '\"')
	{
		(*sublen)++;
		(*str)++;
	}
	flag->w_s = false;
}

void	count_substr(char **str, int *sublen, t_bool *flag)
{
	*sublen = 0;
	if (**str == '\'' && !flag->s_s && !flag->d_s)
		count_single_quote(str, sublen, flag);
	else if (**str == '\"' && !flag->s_s && !flag->d_s)
		count_double_quote(str, sublen, flag);
	else
		count_word(str, sublen, flag);
}
