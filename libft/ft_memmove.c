/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:03:29 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/18 16:27:38 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	1.  copies n bytes from the memory block pointed to 
	by src to the memory block pointed to by dest.
	2. two blocks may overlap
*/
#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char		*de;
	const unsigned char	*sr;
	size_t				i;

	if (!dest && !src)
		return (NULL);
	de = (unsigned char *) dest;
	sr = (const unsigned char *) src;
	if (de < sr)
		while (n--)
			*de++ = *sr++;
	else
	{
		i = n;
		while (i > 0)
		{
			de[i - 1] = sr[i - 1];
			i--;
		}
	}
	return (dest);
}
/*
#include <stdio.h>

int main(void)
{
	char array[6] = "abcde";
	ft_memmove(array + 2, array, 4);
	printf("%s\n", array);
	return (0);
}
*/
