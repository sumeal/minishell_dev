/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 11:33:04 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/12 12:48:51 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

/*
#include <stdio.h>

int	main(void)
{
    char c1 = '5';
    char c2 = 'a';
    
    printf("%c is a digit: %d\n", c1, isdigit(c1));
    printf("%c is a digit: %d\n", c2, isdigit(c2));
    
    return 0;
}
*/