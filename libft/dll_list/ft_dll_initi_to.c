/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dll_initi_to.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 15:23:13 by rileone           #+#    #+#             */
/*   Updated: 2024/04/14 13:49:17 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

static void	ft_dll_clear3(t_dll_list *list)
{
	ft_dll_return_head(&list);
	ft_dll_clear(&list, free);
}

t_dll_list	*ft_dll_initi_to(int len, int value)
{
	t_dll_list	*head;
	t_dll_list	*newnode;
	int			*val;
	int			i;

	i = 0;
	head = NULL;
	while (i < len)
	{
		val = ft_calloc(1, sizeof(int));
		if (!val)
			return (ft_dll_clear3(head), NULL);
		*val = value;
		newnode = ft_dll_new(val);
		if (!newnode)
			return (ft_dll_clear3(head), NULL);
		ft_dll_insert_tail(&head, newnode);
		i++;
	}
	return (head);
}
/* 
int	main(void) {
	t_dll_list *start = ft_dll_initi_to(5, 0);
	int i = 0;
	while (start != NULL) {
		printf("%i -> ", *(int *)(start)->val);
		start = start->next;
		i++;
	}
} */
