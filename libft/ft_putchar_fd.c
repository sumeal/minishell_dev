/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 14:11:15 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/15 12:29:46 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
writes the character c to the file specified by the file descriptor fd. 
The &c is a pointer to the character, 
and 1 indicates that you're writing one byte (one character).
*/
#include "libft.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
/*
int	main(void)
{
	ft_putchar_fd('A', 1);
	ft_putchar_fd('b', 2);
	return (0);
}
*/