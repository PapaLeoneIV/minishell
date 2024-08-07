/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/21 14:02:41 by rileone           #+#    #+#             */
/*   Updated: 2024/07/04 14:01:09 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	g_status_code;

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	g_status_code = 0;
	shell = (t_shell){0};
	(void)argv;
	if (argc != 1)
	{
		write(2, "You need to run the program as : ./minishell\n", 46);
		exit(EXIT_SUCCESS);
	}
	read_from_stdin(&shell, envp);
	return (EXIT_SUCCESS);
}
