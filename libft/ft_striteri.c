/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 13:49:36 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/13 11:05:08 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 iterate over each character of a string and apply a function to each character
 both the character and its index as arguments
*/
void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	unsigned int	i;

	i = 0;
	while (s[i] != '\0')
	{
		f(i, &s[i]);
		i++;
	}
}
/*
#include <stdio.h>

void *test(unsigned int i, char *s)
{
	if (i % 2 == 0)
		*s = 'a';
	else
		*s = 'b';
}

int main(void)
{
	char	s[] = "hello world";
	ft_striteri(s,test);
	printf("the string is : %s\n", s);
	return (0);

}
*/