/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 10:16:32 by jchen2            #+#    #+#             */
/*   Updated: 2025/03/13 20:14:24 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*copy_substr(char *start, int sublen)
{
	char	*substr;
	int		i;

	substr = (char *)malloc(sizeof(char) * (sublen + 1));
	if (!substr)
		return (NULL);
	i = 0;
	while (i < sublen)
	{
		substr[i] = start[i];
		i++;
	}
	substr[i] = '\0';
	return (substr);
}

char	*count_copy_substr(char **str, t_bool *flag)
{
	int		sublen;
	char	*start;
	char	*substr;

	start = *str;
	sublen = 0;
	count_substr(str, &sublen, flag);
	substr = copy_substr(start, sublen);
	return (substr);
}
