/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:03:15 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/18 12:36:06 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	1.  copies n bytes from the memory block pointed to 
	by src to the memory block pointed to by dest.
	2. two blocks should not overlap
*/
#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	unsigned char		*de;
	const unsigned char	*sr;
	size_t				i;

	de = (unsigned char *) dest;
	sr = (const unsigned char *) src;
	i = 0;
	while (i < n)
	{
		de[i] = sr[i];
		i++;
	}
	return (dest);
}
/*
#include <stdio.h>

int main(void)
{
	int s1[10];
	int	s2[10] = {1, 2, 3, 4, 5};

	ft_memcpy(s1, s2, 5*sizeof(int));
	for (int i = 0; i < 5; i++)
	{
		printf("%d", s1[i]);
	}
	printf("\n");
}
*/
