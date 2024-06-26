/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgori <fgori@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 14:36:02 by rileone           #+#    #+#             */
/*   Updated: 2024/05/28 11:47:30 by fgori            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/lexer.h"

void print_token_type_and_value(t_token *head)
{
	t_token *current = head;
	while (current != NULL) {
			if (current->type == WORD_TOKEN) {
				printf("WORD_TOKEN ");
				// Handle WORD_TOKEN
			} else if (current->type == PIPE_TOKEN) {
				printf("PIPE_TOKEN ");
				// Handle PIPE_TOKEN
			} else if (current->type == GREATER_TOKEN) {
				printf("GREATER_TOKEN ");
				// Handle GREATER_TOKEN
			} else if (current->type == REDIR_OUT_TOKEN) {
				printf("REDIR_OUT_TOKEN ");
				// Handle REDIR_OUT_TOKEN
			} else if (current->type == LESSER_TOKEN) {
				printf("LESSER_TOKEN ");
				// Handle LESSER_TOKEN
			} else if (current->type == HEREDOC_TOKEN) {
				printf("HEREDOC_TOKEN ");
				// Handle HEREDOC_TOKEN
			} else if (current->type == SING_QUOTES_TOKEN) {
				printf("SING_QUOTES_TOKEN ");
				// Handle SING_QUOTES_TOKEN
			} else if (current->type == DOUBLE_QUOTES_TOKEN) {
				printf("DOUBLE_QUOTES_TOKEN ");
				// Handle DOUBLE_QUOTES_TOKEN
			} else if (current->type == DOLLAR_TOKEN) {
				printf("DOLLAR_TOKEN ");
				// Handle DOLLAR_TOKEN
			} else if (current->type == WHITESPACE_TOKEN) {
				printf("WHITESPACE_TOKEN ");
				// Handle WHITESPACE_TOKEN
			}
			else if (current->type == HERDOC_FILENAME_WITHQUOTES) {
				printf("HEREDOC_FILENAME_WITHOQUOTES ");
				// Handle WHITESPACE_TOKEN
			}
			printf("{Value: %s}\n", current->value);
			current = current->next;
		}
}

/***Crea una lista di token per una prima scrematura degli argomenti, i vari token sono in lexer.h
 * E' una finite state machine, ogni stato (GENERAL, SQUOTES/DQUOTES, DOLLARSIGN) possiede le proprie regole
 * per capire quando e come tagliare la stringa.
*/


int create_token_list(char *stringa, t_shell *shell, t_parser *pars)
{
	if (stringa == NULL || shell == NULL || pars == NULL)
		return (ERROR);
	while(stringa[pars->count] == ' ')  											
		pars->count++;
	pars->start = pars->count;
	while (stringa[pars->count] != '\0')
	{
		pars->char_type = get_char_type(stringa, pars, pars->count); 												
		if (pars->state == STATE_GENERAL)	
			general_state_handler(stringa, pars);  														
		else if ((pars->state == STATE_SQUOTE && pars->char_type == SQUOTES_CHAR) 
		|| (pars->state == STATE_DQUOTE && pars->char_type == DQUOTES_CHAR)) 							                                                                                                       
			quoted_state_handler(stringa, pars);
		else if (pars->state == STATE_DOLLAR && ((pars->char_type != REG_CHAR && pars->char_type != DIGIT_CHAR)
		|| (pars->char_type == DIGIT_CHAR && stringa[pars->count - 1] == '$')))
			dollar_state_handler(stringa, pars, shell);										
		if (stringa[pars->count + 1] == '\0' && !slice_end_token(stringa, pars, shell))
				return (ERROR);																			
		pars->count++;
	}
	(void)shell;
	return (SUCCESS);
}

/**Tutti i token consecutivi che sono WHITESPACE vengo rimossi 
 * per un parsing piu comodo successivamente
*/

void trim_middleline_whitespaces(t_parser *pars)
{
	t_token *ptr;
	t_token *tmp;
	t_token *current;

	ptr = pars->head;
	while(ptr)
	{
		if (ptr && ptr->type == WHITESPACE_TOKEN)
		{
			tmp = ptr->next;
			while(tmp && tmp->type == WHITESPACE_TOKEN)
			{
				current = tmp;
				tmp = tmp->next;
				free(current->value);
				free(current); //devo liberare meglio tutto il token anche value credo 
			}
			ptr->next = tmp;
			if (tmp)
				tmp->prev = ptr;
		}
		ptr = ptr->next;
	}
}



/**Dopo aver creato una prima lista di token,
 * la lista viene refinita:
 * - tokenizzazione dei token double quotes con annessa espansione
 * - i token che sono vicini senza spazi vengo joinati insieme (es: "/home/"$USER) ==> priva tokenizzati separatamente e rimessi insieme
 * - - rimuovo tutti gli spazi consecutivi nel mezzo della stringa di input
 * - rimuovo le env che sono state espanse a NULL in quanto non valide
*/




t_token *tokenize_input(char *input, t_shell *shell)
{
	t_parser pars;
	t_token *head;

	pars = (t_parser){0};
	if (create_token_list(input, shell, &pars) == ERROR)
		return (free_tokens(pars.head), NULL);
	unpack_quoted_tokens(&pars, shell);
	join_tokens_values_when_no_space_between(&pars);
	//print_token_type_and_value(pars.head); 
	trim_middleline_whitespaces(&pars);
	remove_null_tokens(&pars);
 	//token_print(pars.head);								//to use tester.py enable this function
	head = pars.head;
	return (head);
}