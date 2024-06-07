/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 16:23:54 by fgori             #+#    #+#             */
/*   Updated: 2024/06/07 16:29:23 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_env	*ft_lstlast_(t_env *lst)
{
	while (lst && lst-> next != NULL)
		lst = lst->next;
	return (lst);
}

void	add_node_to_env_struct(t_env **env_list, t_env *new)
{
	t_env	*temp;

	if (!*env_list)
	{
		*env_list = new;
		return ;
	}
	if (!new)
		return ;
	temp = ft_lstlast_(*env_list);
	temp->next = new;
	new->prev = temp;
	new->next = NULL;
}

t_env	*lst_new_env(char *envp_line, char **en)
{
	t_env	*new;
	int		int_split;

	if (!envp_line)
		return (NULL);
	new = ft_calloc(1, sizeof(t_env));
	if (!new)
		return (NULL);
	int_split = ft_strchri(envp_line, '=');
	if (int_split != -1)
	{
		new->head = ft_substr(envp_line, 0, int_split);
		new->body = ft_substr(envp_line, int_split + 1, ft_strlen(envp_line));
		new->esistence = 0;
		if (ft_strncmp(new->head, "_", ft_strlen(new->head)) == 0)
			new->esistence = -1;
	}
	else 
	{
		new->head = ft_strdup(envp_line);
		new->esistence = 1;
	}
	new->env_mtx = en;
	new->next = NULL;
	return (new);
}

t_env	**get_env_info(char *envp_mtx[])
{
	t_env	**env_struct;
	t_env	*node;
	int		counter;

	counter = 0;
	env_struct = ft_calloc(1, sizeof(t_env *));
	while (envp_mtx[counter])
	{
		node = lst_new_env(envp_mtx[counter], envp_mtx);
		if (node == NULL)
		{
			clean_env_lst(env_struct);
			exit (EXIT_FAILURE);
		}
		add_node_to_env_struct(env_struct, node);
		counter++;
	}
	return (env_struct);
}