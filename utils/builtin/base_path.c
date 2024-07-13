/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 16:21:45 by fgori             #+#    #+#             */
/*   Updated: 2024/07/13 22:01:43 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	pwd_path(void)
{
	char	*path;

	path = getcwd(NULL, 0);
	ft_printf("%s\n", path);
	free(path);
	return (0);
}

static void	check_n_flag(char *str, short *n_flag, int *pos)
{
	int	i;

	i = 1;
	while (str[i] != '\0')
	{
		if (str[i] != 'n')
		{
			*pos = 1;
			return ;
		}
		i++;
	}
	*n_flag = false;
	*pos = 2;
	return ;
}

int	echo_path(char **str)
{
	int		i;
	short	n_flag;

	i = 1;
	n_flag = true;
	if (!str)
		return (2);
	if (mtx_count_rows(str) > 1)
	{
		if (ft_strncmp(str[1], "-n", 2) == 0)
			check_n_flag(str[1], &n_flag, &i);
		while (str[i])
		{
			ft_printf("%s", str[i]);
			if (str[i + 1])
				ft_printf(" ");
			i++;
		}
	}
	if (n_flag)
		ft_printf("\n");
	return (0);
}

int	env_path(t_env **env, t_command *cmd)
{
	t_env	*tmp;

	tmp = (*env);
	while (tmp)
	{
		if (mtx_count_rows(cmd->cmd) > 1)
		{
			write_exit(cmd->cmd[1], " : No such file or directory\n", NULL);
			return (127);
		}
		if (tmp->esistence <= 0)
			ft_printf("%s=%s\n", tmp->head, tmp->body);
		tmp = tmp->next;
	}
	return (0);
}
