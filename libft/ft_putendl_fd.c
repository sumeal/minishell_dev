/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 14:34:43 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/15 12:29:32 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
The ft_putendl_fd function outputs a string to the specified file descriptor, 
appending a newline character at the end
*/
#include "libft.h"

void	ft_putendl_fd(char *s, int fd)
{
	if (s == NULL)
		return ;
	while (*s)
	{
		write(fd, s, 1);
		s++;
	}
	write(fd, "\n", 1);
}
/*
int main(void)
{
	ft_putendl_fd("hello", 1);
	ft_putendl_fd("hello world", 2);

	return (0);
}
*/