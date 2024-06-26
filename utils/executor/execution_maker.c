/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_maker.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fgori <fgori@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 10:52:33 by fgori             #+#    #+#             */
/*   Updated: 2024/05/31 14:45:43 by fgori            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	list_of_out(t_redir **dir)
{
	int	fd;

	if ((*dir)->type_of_redirection == GREATER_TOKEN)
	{
		while ((*dir) && (*dir)->type_of_redirection == GREATER_TOKEN)
		{
			fd = open((*dir)->filename,  O_TRUNC | O_CREAT | O_RDWR, 0777);
			if (fd < 0)
				break ;
			if ((*dir)->next && (*dir)->next->type_of_redirection == GREATER_TOKEN)
				close(fd);
			(*dir) = (*dir)->next;
		}
	}
	else
	{
		while ((*dir) && (*dir)->type_of_redirection == REDIR_OUT_TOKEN)
		{
			fd = open((*dir)->filename,  O_APPEND | O_CREAT | O_RDWR, 0777);
			if (fd < 0)
				break ;
			if ((*dir)->next && (*dir)->next->type_of_redirection == REDIR_OUT_TOKEN)
				close(fd);
			(*dir) = (*dir)->next;
		}
	}
	if (fd < 0)
		{
			printf("impossible to open: %s\n", (*dir)->filename);
			return (-1);
		}
	return (fd);
}

int	list_of_in(t_redir **dir)
{
	int	fd;
	
	while ((*dir) && (*dir)->type_of_redirection == LESSER_TOKEN)
	{
		fd = open((*dir)->filename, O_RDONLY, 0777);
		if (fd < 0)
		{
			printf("%s: No such file or directory\n", (*dir)->filename);
			(*dir) = (*dir)->next;
			return(-1);
		}
		if ((*dir)->next && (*dir)->next->type_of_redirection == LESSER_TOKEN)
			close(fd);
		(*dir) = (*dir)->next;   
	}
	return (fd);
}

void	freeall(char **mat)
{
	int	n;

	n = 0;
	while (mat[n])
	{
		free(mat[n]);
		mat[n] = NULL;
		n++;
	}
	free(mat);
}

int	ft_biltin(char **tmp, t_env **lst)
{
	int i;

	i = -1;
	if (ft_strncmp(tmp[0], "cd", 2) == 0)
	{
		i = cd_path(tmp);
	}
	else if (ft_strncmp(tmp[0], "pwd", 3) == 0)
	{
		i = pwd_path();
	}
	else if (ft_strncmp(tmp[0], "echo", 4) == 0)
		i = echo_path(tmp);
	else if (ft_strncmp(tmp[0], "env", 3) == 0)
		i = env_path(lst);
	else if (ft_strncmp(tmp[0], "export", 6) == 0)
		i = export_path(lst, tmp);
	else if (ft_strncmp(tmp[0], "unset", 5) == 0)
		i = unset_path(lst, tmp);
	return (i);
}

int	make_things(char **cmd, t_env *path, t_env **env)
{
	char	**open_path;
	char	*tmp;
	char	*supp;
	int		i;

	i = 0;
	if (ft_biltin(cmd, env) == -1)
	{
		open_path = ft_split(path->body, ':');
		while (open_path[i])
		{
			tmp = ft_strjoin(open_path[i], "/");
			supp = ft_strjoin(tmp, cmd[0]);
			free(tmp);
			if (access(supp, F_OK | X_OK) == 0)
				break ;
			free (supp);
			i++;
		}
		freeall(open_path);
		if (!supp)
			return (perror("access don't replies"), -1);
		if (execve(supp, cmd, path->env_mtx) < 0)
			perror("ERROR\n execve don't replies");
		free(supp);
	}
	return (1);
}


// da lavorare non completa!!!!!
int	execution(t_command *cmd, t_env **env, t_shell *shell)
{
	int		pip[2];
	int		tm_i;
	int		tm_ou;
	t_redir	**tmp;
	/**Note da osservare credo:
	 * - l heredoc deve essere la prima cosa che va eseguita da quello che ho visto, (va cercato se c e' un 
	 * 			HEREDOC_TOKEN con associato WORD_TOKEN/HEREDOC_TOKEN_WITH_QUOTES) e va startato il prompt 
	 * 			di heredoc prima di ogni altro comando. Il contenuto dell heredoc va poi visto se essere espanso oppure no.
	 * - 
	*/
	(void)shell;
	tm_i = cmd->in;
	tm_ou = cmd->out;
	if (cmd->next != NULL)
		if (pipe(pip) < 0)
			return(perror ("ERROR while opening the pipe\n"), ERROR);
	if (cmd->next)
	{
		cmd->out = pip[1];
		cmd->next->in = pip[0];
	}
	tmp = cmd->redirection_info;
	while (tmp)
	{
		if (!(*tmp))
			break ;
		if ((*tmp)->type_of_redirection  == GREATER_TOKEN
			|| (*tmp)->type_of_redirection  == REDIR_OUT_TOKEN)
			cmd->out = list_of_out(&(*tmp));
		else if ((*tmp)->type_of_redirection  == LESSER_TOKEN)
			cmd->in = list_of_in(&(*tmp));
		/* else
			heardoc_path(); */
		if (cmd->in == -1 || cmd->out == -1)
			return (ERROR);	
	}
	dup2(cmd->in, 0);
	dup2(cmd->out, 1);
	cmd->fork_id = fork();
	if (cmd->fork_id == 0)
	{
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
            close(pip[0]);
        if (cmd->prev)
            close(pip[1]);

 		make_things(cmd->cmd, find_node(env, "PATH"), env);
		exit(0);
	}
	else
	{
		if (cmd->in != 0)
            close(cmd->in);
        if (cmd->out != 1)
            close(cmd->out);
	}
	return SUCCESS;
}

void	open_redir(t_redir **redir)
{
	t_redir	**tmp;
	
	tmp = redir;
	while (tmp)
	{
		if (!(*tmp))
			break ;
		if ((*tmp)->type_of_redirection  == GREATER_TOKEN
			|| (*tmp)->type_of_redirection  == REDIR_OUT_TOKEN)
			close(list_of_out(&(*tmp)));
		else if ((*tmp)->type_of_redirection  == LESSER_TOKEN)
			close(list_of_in(&(*tmp)));
		/* else
			heardoc_path(); */
	}
}

int	exit_path(t_command *cmd, t_shell *shell)
{
	unsigned int	exit_status;
	int 			i;

	i = 0;
	exit_status = 0;
	open_redir(cmd->redirection_info);
	if (mtx_count_rows(cmd->cmd) > 2)
		return(perror("ERROR\nto many argument"), -1);
	if (cmd->cmd[1])
	{
		while (cmd->cmd[1][i])
		{
			if (!ft_isdigit(cmd->cmd[1][i]))
				return(perror("ERROR\nalpha in exit status"), ERROR);
			i++;
		}
		exit_status = (unsigned int)ft_atoi(cmd->cmd[1]);
		if (exit_status > 255)
			exit_status %= 256;
	
	}
	clean_all(shell);
	return (exit_status);
}

int	execute_cmd(t_shell *shell)
{
	t_command	*cmd;
	int			tm_in;
	int			tm_out;

	cmd = (*shell->cmd_info);
	tm_in = dup(0);
	tm_out = dup(1);
	if (!(cmd->next) && ft_strncmp(cmd->cmd[0], "exit", ft_strlen(cmd->cmd[0])) == 0)
	{
		close (tm_in);
		close (tm_out);
		exit(exit_path(cmd, shell));
	}
	while(cmd)
	{
		if (execution(cmd, shell->env, shell) == ERROR)
		{
			
			perror("ERROR");
			return (ERROR);
		}
		cmd = cmd->next;
	}
	while (waitpid(-1, NULL, 0) > 0)
		;
	dup2(tm_in, 0);
	dup2(tm_out, 1);
	close(tm_in);
	close(tm_out);
	return (SUCCESS);
}
