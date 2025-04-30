/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/09 10:06:11 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/13 11:26:43 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_tolower(int c)
{
	if (c >= 'A' && c <= 'Z')
		return (c + 'a' - 'A');
	return (c);
}
/*
#include <stdio.h>

int main(void)
{
	printf("%c\n", ft_tolower(65));
	printf("%d\n", ft_tolower(3456));
	return (0);
}
*/