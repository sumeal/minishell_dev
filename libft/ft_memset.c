/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:03:50 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/16 15:21:29 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
initial the string of set the string to a secified value;
n: the number of bytes to be set to the value
c: The byte value to be set(passed an int but cast to an unsigned char)
*/
#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*ptr;
	size_t			i;

	ptr = (unsigned char *) s;
	i = 0;
	while (i < n)
	{
		ptr[i] = (unsigned char)c;
		i++;
	}
	return (s);
}
/*
#include <stdio.h>

int	main(void)
{
	unsigned char	x[20];

	ft_memset(x, 'A', 10);
	x[10] = '\0';
	printf("%s\n", x);
	return (0);
}
*/