/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_maker.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                            	    +#+#+#+#+#+   +#+         */
/*   Created: 2024/06/25 11:20:59 by fgori             #+#    #+#             */
/*   Updated: 2024/06/28 11:30:09 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	make_things(t_command *cmd, t_env *path, t_env **env, t_shell *shell)
{
	char	*supp;
	char	**mtx;
	char	**tmp_cmd;
	char	**tmp_env;

	mtx = cmd->cmd;
	if (ft_biltin(cmd, env, shell) == -1)
	{
		tmp_cmd = mtx_dup(mtx, mtx_count_rows(mtx));
		tmp_env = mtx_dup((*env)->env_mtx, mtx_count_rows((*env)->env_mtx));
		supp = take_path(path, tmp_cmd, cmd, shell);
		if (supp == NULL)
			return (multi_freeall(tmp_env, tmp_cmd, shell, NULL));
		clean_all(shell, 1);
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		if (execve(supp, tmp_cmd, tmp_env) < 0)
		{
			exev_error(shell, supp);
			return (multi_freeall(tmp_env, tmp_cmd, NULL, supp));
		}
		free(supp);
	}
	return (clean_all(shell, 1));
}

void	child_process(t_shell *shell, t_command *cmd)
{
	t_env	*tmp;

	if (cmd->fd_change == 1 || cmd->fd_change == 3)
		dup2(cmd->in, 0);
	if (cmd->fd_change == 1 || cmd->fd_change == 3)
		close(cmd->in);
	if (cmd->fd_change == 2 || cmd->fd_change == 3)
		dup2(cmd->out, 1);
	if (cmd->fd_change == 2 || cmd->fd_change == 3)
		close(cmd->out);
	tmp = find_node(shell->env, "PATH");
	if (tmp == NULL && access(cmd->cmd[0], F_OK) != 0 && !is_a_biltin(cmd->cmd))
		write_clean(cmd->cmd[0], shell);
	else if (cmd->fd_change >= 0)
	{
		make_things(cmd, tmp, shell->env, shell);
		if (g_status_code == 126 || g_status_code == 130)
			shell->status = (int [2]){130, 126}[g_status_code == 126];
	}
	else
		clean_all(shell, 1);
	tm_close(shell->s_pip[0], shell->s_pip[1], 0);
	exit(shell->status);
}

static void	fork_and_ecseve(t_shell *shell, t_command *cmd)
{
	if (cmd->cmd)
		cmd->fork_id = fork();
	if (cmd->fork_id == 0)
	{
		tm_close(shell->s_pip[0], shell->s_pip[1], 0);
		if (cmd->next)
			close(cmd->pip[0]);
		if (cmd->prev)
			close(cmd->pip[1]);
		child_process(shell, cmd);
	}
	else
	{
		if (cmd->here)
			unlink(cmd->here);
		if (cmd->next)
			close(cmd->pip[1]);
		if (cmd->in != 0 && cmd->in != -1)
			close(cmd->in);
		if (cmd->out != 1 && cmd->out != -1)
			close(cmd->out);
	}
}

int	execution(t_command *cmd, t_env **env, t_shell *shell)
{
	dup2(cmd->in, 0);
	dup2(cmd->out, 1);
	g_status_code = 0;
	signal(SIGINT, handle_signal_block);
	if (cmd->cmd && is_a_biltin(cmd->cmd) && !cmd->next && cmd->cmd_id == 0
		&& cmd->fd_change >= 0)
		return (ft_biltin(cmd, env, shell));
	fork_and_ecseve(shell, cmd);
	return (SUCCESS);
}

int	execute_cmd(t_shell *shell)
{
	t_command	*cmd;

	cmd = (*shell->cmd_info);
	shell->s_pip[0] = dup(0);
	shell->s_pip[1] = dup(1);
	if (cmd && !(cmd->next) && cmd->cmd && ft_strncmp(cmd->cmd[0], "exit", 5) == 0)
	{
		if (tm_close(shell->s_pip[0], shell->s_pip[1], 0)
			&& exit_path(cmd, shell) == 1)
			return (ERROR);
	}
	if (make_redir(shell, cmd) == 2)
		return (tm_close(shell->s_pip[0], shell->s_pip[1], 1), ERROR);
	while (cmd)
	{
		if (cmd->cmd && execution(cmd, shell->env, shell) == ERROR)
			return (tm_close(shell->s_pip[0], shell->s_pip[1], 1), ERROR);
		else
			close(cmd->pip[1]);
		if (cmd->here)
			unlink(cmd->here);
		cmd = cmd->next;
	}
	return (take_last_pid(shell), tm_close(shell->s_pip[0],
			shell->s_pip[1], 1), SUCCESS);
}
