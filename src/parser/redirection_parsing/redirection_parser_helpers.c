/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_parser_helpers.c                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/22 22:27:54 by rileone           #+#    #+#             */
/*   Updated: 2024/07/17 17:41:18 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

t_redir	*new_redir_node(int type, char *value, int flag)
{
	t_redir	*redir_node;

	redir_node = ft_calloc(1, sizeof(t_redir));
	if (type != HEREDOC_TOKEN)
		redir_node->filename = ft_strdup(value);
	else
		redir_node->filename = ft_strjoin(value, "_h");
	redir_node->type_of_redirection = type;
	if (!flag)
		redir_node->heredoc_expansion = 0;
	else
		redir_node->heredoc_expansion = 1;
	redir_node->next = NULL;
	return (redir_node);
}

void	add_back_redirections(t_redir **lst, t_redir *node)
{
	t_redir	*ptrlst;

	if (*lst == NULL)
	{
		*lst = node;
		return ;
	}
	ptrlst = *lst;
	while (ptrlst->next != NULL)
		ptrlst = ptrlst->next;
	ptrlst->next = node;
}

t_command	*new_command(int counter)
{
	t_command	*cmd_node;

	cmd_node = ft_calloc(1, sizeof(t_command));
	cmd_node->cmd = NULL;
	cmd_node->cmd_id = counter;
	cmd_node->fork_id = 1;
	cmd_node->in = dup(STDIN_FILENO);
	cmd_node->here = NULL;
	cmd_node->out = dup(STDOUT_FILENO);
	cmd_node->save_rd[0] = cmd_node->in;
	cmd_node->save_rd[1] = cmd_node->out;
	cmd_node->fd_change = 0;
	cmd_node->cat = 0;
	cmd_node->redirection_info = NULL;
	cmd_node->next = NULL;
	cmd_node->prev = NULL;
	return (cmd_node);
}

void	add_back_commands(t_command **lst, t_command *node)
{
	t_command	*ptrlst;

	if (*lst == NULL)
	{
		*lst = node;
		return ;
	}
	ptrlst = *lst;
	while (ptrlst->next != NULL)
		ptrlst = ptrlst->next;
	ptrlst->next = node;
	node->prev = ptrlst;
}

char	**from_lst_to_mtx(t_token *head)
{
	int		i;
	int		len;
	char	**mtx;
	t_token	*ptr;

	ptr = head;
	i = 0;
	len = 0;
	while (ptr)
	{
		ptr = ptr->next;
		len++;
	}
	if (len == 0)
		return (NULL);
	ptr = head;
	mtx = ft_calloc(len + 1, sizeof(char *));
	while (i < len)
	{
		mtx[i] = ft_strdup(ptr->value);
		i++;
		ptr = ptr->next;
	}
	return (mtx);
}
