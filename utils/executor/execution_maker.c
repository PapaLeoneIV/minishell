/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_maker.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rileone <rileone@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/06/04 11:110:07 by rileone          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../includes/minishell.h"


int	ft_biltin(char **tmp, t_env **lst)
{
	int i;

	i = -1;
	if (ft_strncmp(tmp[0], "cd", ft_strlen(tmp[0])) == 0)
	{
		i = cd_path(tmp, lst);
	}
	else if (ft_strncmp(tmp[0], "pwd", ft_strlen(tmp[0])) == 0)
	{
		i = pwd_path();
	}
	else if (ft_strncmp(tmp[0], "echo", ft_strlen(tmp[0])) == 0)
		i = echo_path(tmp);
	else if (ft_strncmp(tmp[0], "env", ft_strlen(tmp[0])) == 0)
		i = env_path(lst);
	else if (ft_strncmp(tmp[0], "export", ft_strlen(tmp[0])) == 0)
		i = export_path(lst, tmp);
	else if (ft_strncmp(tmp[0], "unset", ft_strlen(tmp[0])) == 0)
		i = unset_path(lst, tmp);
	return (i);
}

char	*ft_access(char **open_path, char *cmd)
{
	char	*tmp;
	char	*supp;
	int		i;

	i = 0;
	if (access(cmd, F_OK | X_OK) == 0)
			return (cmd);
	while (open_path[i])
	{
		tmp = ft_strjoin(open_path[i], "/");
		supp = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(supp, F_OK | X_OK) == 0)
			return (supp);
		free (supp);
		i++;
	}
	return (NULL);
}

int	make_things(char **cmd, t_env *path, t_env **env, t_shell *shell)
{
	char	**open_path;
	char	*supp;
	char 	**tmp_cmd;
	char	**tmp_env;
	
	signal(SIGQUIT, SIG_DFL);
	/**se il comando e' sbagliato al momento ritorniamo uno da questa funzione senza
	 * gestire l errore.(minishell/tester/LEAK_test_outputs/memory_leak_report_7.txt)
	*/
	if (ft_biltin(cmd, env) == -1)
	{
		tmp_cmd = mtx_dup(cmd, mtx_count_rows(cmd));
		tmp_env = mtx_dup(path->env_mtx, mtx_count_rows(path->env_mtx));
		open_path = ft_split(path->body, ':');
		supp = ft_access(open_path, tmp_cmd[0]);
		if (!supp)
		{
			perror("ERROR\nunfinded path");
			freeall(tmp_cmd);
			freeall(tmp_env);
			return(freeall(open_path), ERROR);
		}
		freeall(open_path);
		clean_all(shell, 1);
		if (execve(supp, tmp_cmd, tmp_env) < 0)
			perror("ERROR\n execve don't replies");
		free(supp);
	}
	return (ERROR);
}

int	is_a_biltin(char **tmp)
{
	if (ft_strncmp(tmp[0], "cd", ft_strlen(tmp[0])) == 0
			|| ft_strncmp(tmp[0], "pwd", ft_strlen(tmp[0])) == 0
			|| ft_strncmp(tmp[0], "echo", ft_strlen(tmp[0])) == 0
			|| ft_strncmp(tmp[0], "env", ft_strlen(tmp[0])) == 0
			|| ft_strncmp(tmp[0], "export", ft_strlen(tmp[0])) == 0
			|| ft_strncmp(tmp[0], "unset", ft_strlen(tmp[0])) == 0
			|| ft_strncmp(tmp[0], "exit", ft_strlen(tmp[0])) == 0)
			return (1);
	return (0);
}

void	child_process(t_shell *shell, t_command *cmd, int tm_i, int tm_ou)
{
	t_env	*tmp;

	if (cmd->in != tm_i)
	{
		dup2(cmd->in, 0);
		close(cmd->in);
	}
	if (cmd->out != tm_ou)
	{
		dup2(cmd->out, 1);
		close(cmd->out);
	}
	// Close unused pipe ends in the child process
	if (cmd->next)
		close(cmd->pip[0]);
	if (cmd->prev)
		close(cmd->pip[1]);
	tmp = find_node(shell->env, "PATH");
	if (tmp == NULL && access(cmd->cmd[0], F_OK | X_OK) != 0)
	{
		perror("unvalid Path\n");
		clean_all(shell, 1);
	}
	else
	{
		if (make_things(cmd->cmd, tmp, shell->env, shell) == ERROR)
		{
			clean_all(shell, 1);
			dup2(0, 0);
			dup2(1, 1);
		}
	}
	exit(EXIT_SUCCESS);
}

void	fork_and_ecseve(t_shell *shell, t_command *cmd, int tm_i, int tm_ou)
{
	cmd->fork_id = fork();
	if (cmd->fork_id == 0)
		child_process(shell, cmd, tm_i, tm_ou);
	else
	{
		if (cmd->here)
			{
				unlink(cmd->here);
			}
		if (cmd->in != 0)
            close(cmd->in);
        if (cmd->out != 1)
            close(cmd->out);
	}
}

// da lavorare non completa!!!!!
int	execution(t_command *cmd, t_env **env, t_shell *shell)
{
	int		tm_i;
	int		tm_ou;

	tm_i = cmd->in;
	tm_ou = cmd->out;
	if (set_pip(cmd, cmd->pip) == ERROR)
		return (ERROR);
	if (open_redir(cmd, shell) == ERROR)
		return (ERROR);
	dup2(cmd->in, 0);
	dup2(cmd->out, 1);
	if (is_a_biltin(cmd->cmd) && !cmd->next && cmd->cmd_id == 0)
		return (ft_biltin(cmd->cmd, env));
	fork_and_ecseve(shell, cmd, tm_i, tm_ou);
	return SUCCESS;
}

int	execute_cmd(t_shell *shell)
{
	t_command	*cmd;
	int			status;
	int			tm_in;
	int			tm_out;

	cmd = (*shell->cmd_info);
	tm_in = dup(0);
	tm_out = dup(1);
	if (!(cmd->next) && ft_strncmp(cmd->cmd[0],
			"exit", ft_strlen(cmd->cmd[0])) == 0)
	{
		tm_close(tm_in,tm_out, 0);
		if (exit_path(cmd, shell) == 300)
			return (ERROR);
	}
	while(cmd)
	{
		if (execution(cmd, shell->env, shell) == ERROR)
			return (tm_close(tm_in,tm_out, 1), ERROR);
		cmd = cmd->next;
	}
	while (waitpid(-1, &status, 0) > 0)
		printf("-----%d", status);
	tm_close(tm_in,tm_out, 1);
	return (SUCCESS);
}
