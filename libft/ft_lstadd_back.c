/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/10 17:25:47 by jchen2            #+#    #+#             */
/*   Updated: 2024/11/13 10:19:58 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 add the node into list.
 put it behind the list
*/
#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*last;

	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = *lst;
	while (last->next != NULL)
		last = last->next;
	last->next = new;
}
/*
#include <stdlib.h>

t_list	*ft_lstnew(void *content)
{
	t_list	*lstnew;

	lstnew = (t_list *)malloc(sizeof(t_list));
	if (!lstnew)
		return (NULL);
	lstnew->content = content;
	lstnew->next = NULL;
	return (lstnew);
}

#include <stdio.h>

int	main(void)
{
	t_list *head = NULL;
	t_list *node1 = ft_lstnew("first node");
	t_list *node2 = ft_lstnew("second node");
	if (!node1 || !node2)
		return (1);
	ft_lstadd_back(&head, node1);
	ft_lstadd_back(&head, node2);
	t_list *current = head;
	while (current)
	{
		printf("node front: %s\n", (char *)current->content);
		current = current->next;
	}
	free(node1);
	free(node2);
	return (0);
}
*/