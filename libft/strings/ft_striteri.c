/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <riccardo.leone@student.42fir      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 15:23:13 by rileone           #+#    #+#             */
/*   Updated: 2024/04/02 15:23:18 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	unsigned int	i;

	i = 0;
	if (!s || !f)
		return ;
	while (i < (unsigned int)ft_strlen(s))
	{
		(*f)(i, s + i);
		i++;
	}
}

/*
void	my_test_func(unsigned int i, char *c)
{
    printf("Index: %u\n", i);
    *c = ft_toupper(*c);
}

int	main(void)
{
	char	str[];

    str[] = "hello, world!";
    printf("Original string: %s\n", str);
    ft_striteri(str, my_test_func);
    printf("Modified string: %s\n", str);
    return (0);
}
*/
