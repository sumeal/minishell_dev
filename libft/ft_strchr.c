/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:04:23 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/15 12:28:39 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	 searches for the first occurrence of a character in a string
	 locating a specific character within a string or checking if
	  a character exists in it.
*/
#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if (*s == (char) c)
			return ((char *)s);
		s++;
	}
	if (c == '\0')
		return ((char *)s);
	return (NULL);
}
/*
#include <stdio.h>

int main()
{
	char *str = "hello word!";
	char *result;

	result = ft_strchr(str, 'e');
	printf("%s", result);
}
*/