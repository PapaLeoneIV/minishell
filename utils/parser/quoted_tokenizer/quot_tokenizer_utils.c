
#include "../../includes/lexer.h"

t_token *check_prev(t_token *token)
{
	t_token *prev;

	prev = token->prev;
	if (prev == NULL || prev->type == WHITESPACE_TOKEN)
		return NULL;
	if (prev->type == WORD_TOKEN || prev->type == DOUBLE_QUOTES_TOKEN
	|| prev->type == SING_QUOTES_TOKEN || prev->type == DOLLAR_TOKEN
	|| prev->type == HERDOC_FILENAME_WITHQUOTES)
		return prev; 
	return NULL;
}

void join_tokens(t_token **node, t_token **prev)
{

	(*prev)->value = ft_strjoin((*prev)->value, (*node)->value);
	if((*node)->next)
		(*node)->next->prev = (*prev);
	else
		(*prev)->next = NULL;
	(*prev)->next = (*node)->next;
	free((*node)->value);
	free(*node);
	*node = NULL;
	*node = *prev;
}

void join_tokens_values_when_no_space_between(t_parser *pars)
{
	/**qui devo aggiungere una logica per cui se il primo dei token che 
	 * viene joinato e' un token di tipo heredoc_quotes devo mantenere questo datatype
	 * e non convertirlo nel type del token successivo 
	 * 
	*/
	t_token *prev;
	t_token *ptr;
	ptr = pars->head;
	while(ptr != NULL)
	{
		prev = check_prev(ptr);
		if (!prev || (prev && prev->type != HERDOC_FILENAME_WITHQUOTES 
		&& prev->value && prev->value[0] == '\0'))
		{
			ptr = ptr->next;
			continue;
		}
		if (ptr && (ptr->type == WORD_TOKEN || ptr->type == SING_QUOTES_TOKEN 
		|| ptr->type == DOUBLE_QUOTES_TOKEN || ptr->type == DOLLAR_TOKEN ) && prev)
		{
			join_tokens(&ptr, &prev);
		}
		ptr = ptr->next;
	}
}

char *join_quoted_token_expansion(t_token *head)
{
	char *out;
	
	out = NULL;
	while(head)
	{
		out = ft_strjoin(out, head->value);
		head = head->next; 
	}
	return out;
}

void unpack_quoted_tokens(t_parser *pars, t_shell *shell)
{
	t_token *ptr;
	t_parser *list;

	ptr = pars->head;
	while(ptr != NULL)
	{
		if (ptr->type == DOUBLE_QUOTES_TOKEN)
		{
  			list = tokenize_quoted_values(ptr, shell);
			if (list == NULL)
			{
				ptr = ptr->next;
				continue;
			}
			else
			{
				free(ptr->value);
				ptr->value = join_quoted_token_expansion(list->head);
			}
		}
  		ptr = ptr->next;
	}
}