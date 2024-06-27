
#include "../../includes/lexer.h"

int	valid_regchar_quoted(char c)
{
	char	*valid_char;
	int		len;
	int		i;

	i = 0;
	valid_char = "_";
	len = ft_strlen(valid_char);
	while (i < len)
	{
		if (ft_charchar(c, valid_char[i]))
			return (1);
		i++;
	}
	return (0);
}
int	get_char_type_quoted(char c)
{
	if (c == ' ')
		return (WHITESPACE_CHAR);
	else if (c == '$')
		return (DOLLAR_CHAR);
	else if (c == '?')
		return (QUESTION_MARK_CHAR);
	else if (ft_isalpha(c) || valid_regchar_quoted(c))
		return (REG_CHAR);
	else if (ft_isdigit(c))
		return (DIGIT_CHAR);
	return (DOLLAR_SPECIAL_CHAR);
}

t_parser	*tokenize_quoted_values(t_token *node, t_shell *shell)
{
	t_parser	*parser;

	if (node && !(node->value))
		return (NULL);
	parser = ft_calloc(1, sizeof(t_parser));
	parser->state = STATE_GENERAL;
	parser->start = 0;
	parser->count = 0;
	parser->head = NULL;
	while (node->value[parser->count])
	{
		parser->char_type = get_char_type_quoted(node->value[parser->count]);
		if (parser->state == STATE_GENERAL)
			general_state_handler_quoted(node->value, parser, shell);
		else if (parser->state == STATE_DOLLAR
				&& ((parser->char_type != REG_CHAR
						&& parser->char_type != DIGIT_CHAR)
					|| (parser->char_type == DIGIT_CHAR
						&& node->value[parser->count - 1] == '$')))
			dollar_state_handler_quoted(node->value, parser, shell);
				//DOLLAR STATE
		if (node->value[parser->count + 1] == '\0')
		{
			if (!slice_end_token(node->value, parser, shell))
			{
				free_tokens(parser->head);
				free(parser);
				return (NULL); //SLICE END TOKEN
			}
		}
		parser->count++;
	}
	if (parser->head == NULL)
	{
		free(parser);
		return (NULL);
	}
	return (parser);
}

void	general_state_handler_quoted(char *stringa, t_parser *pars,
		t_shell *shell)
{
	char	next;

	next = -1;
	if (pars->char_type == WHITESPACE_CHAR || pars->char_type == DOLLAR_CHAR)
	{
		if (stringa[pars->count + 1])
			next = get_char_type(stringa, pars, pars->count + 1);
		if (pars->char_type == DOLLAR_CHAR && next == WHITESPACE_CHAR)
		{
			slice_token_string_doll_spec_case(stringa, pars);
			return ;
		}
		if (pars->count > pars->start)
		//se ho incontrato uno dei carattere nell if precedente posso tagliare la stringa
			slice_token_string(stringa, pars);
		if (pars->char_type == WHITESPACE_CHAR)
			slice_single_char_token(stringa, pars, shell);
		if (pars->char_type == DOLLAR_CHAR)
		//cambio lo state machine per gestire le virgolette
			check_and_change_status(&pars->state, pars->char_type, pars);
	}
}
void	dollar_state_handler_quoted(char *stringa, t_parser *pars,
		t_shell *shell)
{
	/***qui dipende se voglio gestire $1 $2 $3 $? ....*/

	char *status;
	/***per il momento $? viene espanso e risulta uguale a NULL (ERRORE)*/
	if ((pars->count > pars->start && pars->char_type == DIGIT_CHAR
			&& stringa[pars->count - 1] == '$') ||
		(pars->count > pars->start && pars->char_type == QUESTION_MARK_CHAR
				&& stringa[pars->count - 1] == '$'))
	{
		pars->token = token_new(NULL);
		pars->info = (t_token_info){DOLLAR_TOKEN, stringa, pars->start,
			pars->count + 1};
		set_token_values(pars->token, &pars->info);
		if (strcmp(pars->token->value, "$0") == 0)
		{
			free(pars->token->value);
			pars->token->value = ft_strdup("minishell");
		}
		else if (strcmp(pars->token->value, "$?") == 0)
		{
			status = ft_itoa(g_status_code);
			free(pars->token->value);
			pars->token->value = ft_strdup(status);
			free(status);
		}
		else
			pars->token->value = NULL;
		token_add_back(&pars->head, pars->token);
		pars->start = pars->count + 1;
	}
	if (pars->count > pars->start)
	{
		pars->token = token_new(NULL);
		pars->info = (t_token_info){DOLLAR_TOKEN, stringa, pars->start,
			pars->count};
		set_token_values(pars->token, &pars->info);
		if (pars->token->value)
		{
			expand_env_var(&pars->token, &pars->token->value, shell);
			if (pars->token && pars->token->value)
				token_add_back(&pars->head, pars->token);
		}
		else
		{
			free(pars->token);
			pars->token = NULL;
		}
		pars->start = pars->count;
		pars->count--;
	}

	pars->state = STATE_GENERAL;
}