/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_build.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 11:25:20 by fgori             #+#    #+#             */
/*   Updated: 2024/06/25 11:14:01 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exit_path(t_command *cmd, t_shell* shell)
{
	int	argsc;
	int	counter;

	argsc = mtx_count_rows(cmd->cmd);
	counter = 0;
	write(1, "exit\n", 5);
	if (argsc == 1)
		exit(g_status_code);
	else if (argsc == 2)
	{
		if (cmd->cmd[1][0] == '+' || cmd->cmd[1][0] == '-')
			counter++;
		while (cmd->cmd[1][counter])
		{
			if (!ft_isdigit(cmd->cmd[1][counter]))
			{
				g_status_code = 2;
				exit(g_status_code);
			}
			counter++;
		}
		g_status_code = (unsigned char)ft_atoi(cmd->cmd[1]); // atoi ritorna non 0 perr indicare errore
	}
	else
	{
		write(2, "Too many arguments!", 20);
		g_status_code = 1;
	}
	clean_all(shell, 1);
	// printf("status %d\n", g_status_code);
	exit(g_status_code);
}

