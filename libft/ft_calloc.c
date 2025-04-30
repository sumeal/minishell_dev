/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:01:45 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/16 17:46:20 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*arr;
	size_t	i;

	if (nmemb == 0 || size == 0)
		return (malloc(0));
	if (nmemb > SIZE_MAX / size)
		return (NULL);
	arr = malloc(nmemb * size);
	if (!arr)
		return (NULL);
	i = 0;
	while (i < nmemb * size)
	{
		((unsigned char *)arr)[i] = '\0';
		i++;
	}
	return (arr);
}
/*
#include <stdio.h>

int	main(void)
{
	char	*star;
	int		i;

	star = calloc(4, 5);
	i = 0;
	while (i < 20)
	{
		star[i] = 'H';
		i++;
	}
	printf("%s\n", star);
	star = ft_calloc(0, 3);
	printf("what happen: %s\n", star);
	return (0);
}
*/