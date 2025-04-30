/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 14:07:10 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/12 19:00:58 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
To test ascii
*/
int	ft_isascii(int c)
{
	if (c >= 0 && c <= 127)
		return (1);
	else
		return (0);
}
/*
#include <stdio.h>

int	main()
{
	printf("%d\n", ft_isascii(65));
	printf("%d\n", ft_isascii(345));
	return (0);
}
*/