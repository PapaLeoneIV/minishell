/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_path.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgori <fgori@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/18 11:00:32 by fgori             #+#    #+#             */
/*   Updated: 2024/07/15 18:55:28 by fgori            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	solo_export(t_env **lst)
{
	t_env	*tmp;

	tmp = (*lst);
	while (tmp)
	{
		if (tmp->esistence > -1)
		{
			if (tmp->body)
				ft_printf("declare -x %s=\"%s\"\n", tmp->head, tmp->body);
			else
				ft_printf("declare -x %s\n", tmp->head);
		}
		tmp = tmp->next;
	}
}

static void	normal_equal(t_env **lst, char *str)
{
	char	*sup;
	t_env	*tmp;

	if (ft_strchri(str, '=') > 0)
		sup = ft_substr(str, 0, ft_strchri(str, '='));
	else
		sup = ft_substr(str, 0, ft_strlen(str));
	tmp = find_node(lst, sup);
	if (!tmp)
		add_node_to_env_struct(lst, lst_new_env(str, (*lst)->env_mtx));
	else if (ft_strchri(str, '=') > 0)
	{
		free(tmp->body);
		tmp->body = ft_substr(str, ft_strchri(str, '=') + 1, ft_strlen(str));
		tmp->esistence = 0;
	}
	free(sup);
}

static char	*plus_remove(char *str)
{
	char	*head;
	char	*body;
	char	*final;

	head = ft_substr(str, 0, ft_strchri(str, '+'));
	body = ft_substr(str, ft_strchri(str, '+') + 1, ft_strlen(str));
	final = ft_strjoin(head, body);
	free(head);
	free(body);
	return (final);
}

static void	plus_n_equal(t_env **lst, char *str)
{
	t_env	*tmp;
	char	*sup;
	char	*tmp_two;
	char	*body;

	sup = ft_substr(str, 0, ft_strchri(str, '+'));
	tmp = find_node(lst, sup);
	free(sup);
	if (!tmp)
	{
		sup = plus_remove(str);
		add_node_to_env_struct(lst, lst_new_env(sup, (*lst)->env_mtx));
		return (free(sup));
	}
	else
	{
		body = ft_substr(str, ft_strchri(str, '=') + 1, ft_strlen(str));
		tmp_two = ft_strjoin(tmp->body, body);
		free(body);
		free(tmp->body);
		tmp->body = tmp_two;
	}
	if (tmp->esistence == 1)
		tmp->esistence = 0;
}

int	export_path(t_env **lst, char **mtx)
{
	int	i;
	int	x;

	i = 1;
	if (mtx_count_rows(mtx) == 1)
		solo_export(lst);
	while (mtx[i])
	{
		x = check_head(mtx[i]);
		if (!x)
			return (1);
		if (ft_strnstr(mtx[i], "+=", ft_strlen(mtx[i])))
		{
			plus_n_equal(lst, mtx[i]);
		}
		else
			normal_equal(lst, mtx[i]);
		i++;
	}
	return (0);
}
