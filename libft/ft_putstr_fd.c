/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 14:27:44 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/15 12:29:02 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 print out string using write 
*/
#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	if (s == (NULL))
		return ;
	while (*s)
	{
		write(fd, s, 1);
		s++;
	}
}
/*
#include <unistd.h>

int	main(void)
{
	ft_putstr_fd("Hello, world!", 1); 
	ft_putstr_fd("This is an error message.", 2); 
	return (0);
}
*/