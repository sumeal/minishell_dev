/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:02:54 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/19 16:57:47 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	compare two blocks of memory, byte by byte;
	return the difference value;
*/
#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char	*pt1;
	const unsigned char	*pt2;
	size_t				i;

	pt1 = (const unsigned char *) s1;
	pt2 = (const unsigned char *) s2;
	i = 0;
	while (i < n)
	{
		if (pt1[i] != pt2[i])
			return (pt1[i] - pt2[i]);
		i++;
	}
	return (0);
}
/*
#include <stdio.h>

int main()
{
	int s = 133;
	int w = 123;
	
	printf("%d\n", ft_memcmp(&s, &w, 2));
	return (0);
}
*/