/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:05:00 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/16 15:54:30 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
append one string to the end of another, 
ensuring that the destination buffer doesn’t overflow.
size: takes the size of the destination buffer as an argument.
*/
#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dest_len;
	size_t	src_len;
	size_t	i;

	dest_len = 0;
	src_len = 0;
	while (src[src_len] != '\0')
		src_len++;
	if (!dst && size == 0)
		return (src_len);
	while (dst[dest_len] != '\0' && dest_len < size)
		dest_len++;
	if (size <= dest_len || size == 0)
		return (src_len + dest_len);
	i = 0;
	while (src[i] && (dest_len + i) < (size - 1))
	{
		dst[dest_len + i] = src[i];
		i++;
	}
	dst[dest_len + i] = '\0';
	return (dest_len + src_len);
}
/*
#include <stdio.h>

int main(void)
{
	char arr1[10] = "hello";
	char *arr2 = "world";
	printf("%zu\n", ft_strlcat(arr1, arr2, sizeof(arr1)));
	printf("%s\n", arr1);
}
*/