/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:00:24 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/13 11:23:30 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 extracts a substring from a given string 
 starting at a specified position for a specified length.
*/
#include <stdlib.h>
#include <stddef.h>

static size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (*s)
	{
		i++;
		s++;
	}
	return (i);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;

	if (s == NULL)
		return (NULL);
	if (start >= ft_strlen(s))
	{
		substr = (char *)malloc(1);
		if (!substr)
			return (NULL);
		substr[0] = '\0';
		return (substr);
	}
	substr = (char *)malloc(len + 1);
	if (!substr)
		return (NULL);
	i = 0;
	while (s[start] && i < len)
	{
		substr[i] = s[start];
		start++;
		i++;
	}
	substr[i] = '\0';
	return (substr);
}
/*
#include <stdio.h>

int main(void)
{
	char *s = "HELLO THERE";
	char *substr = ft_substr(s, 1, 4);
	printf("%s\n", substr);
	free(substr);
	return (0);
}
*/