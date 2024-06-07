/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 11:41:05 by rileone           #+#    #+#             */
/*   Updated: 2024/06/07 16:29:38 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"


void handle_signal(int signum)
{
	if (signum == SIGINT)
	{
		write(1, "\n", 1);
		rl_on_new_line();	
		rl_replace_line("", 0);
		rl_redisplay();
	}	
}

void handle_ctrl_d(t_shell *shell, char *input, char *path)
{
	clean_env_lst(shell->env);
	if (shell->line)
		free(shell->line);
	free(input);
	free(path);
	rl_clear_history();
	write(1, "\nexit", 5);	//questo ci va? o lo hai usato come controllo?
	exit(EXIT_SUCCESS);
}

void set_signal_handler(struct sigaction *signal_g)
{
	signal_g->sa_handler = handle_signal;
	sigaction(SIGINT, signal_g, NULL);
	signal(SIGQUIT, SIG_IGN);
}