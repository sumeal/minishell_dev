/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:12:22 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/18 15:19:53 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <readline/readline.h>
#include <readline/history.h> // Needed if using add_history()
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>

volatile sig_atomic_t	g_signal = 0;

typedef struct s_commands
{
	char				*cmd;
	char				**args;
	int					argc;
	int					type;
	char				*input_file;
	char				*output_file;
	int					input_fd;
	int					output_fd;
	int					append_mode;
	int					heredoc;
	char				*delimiter;
	int					g_pid;
	struct s_commands	*next;
	struct s_commands	*prev;
}	t_commands;

char	*ft_getenv(char *name, char **envp)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(name);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], name, len) == 0 && envp[i][len] == '=')
			return (&envp[i][len + 1]);
		i++;
	}
	return (NULL);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

void	handle_signal_parent(int signum)
{
	if (signum == SIGINT)
	{
		printf("\n^C\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
		g_signal = 130;
	}
}

void	setup_signal_handlers(struct termios *original_term, struct termios *new_term)
{
	tcgetattr(STDIN_FILENO, original_term);
	*new_term = *original_term;
	new_term->c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, new_term);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, handle_signal_parent);
	signal(SIGQUIT,SIG_IGN);
}

void	handle_signal_child(int signum)
{
	if (signum == SIGINT)
		ft_putstr_fd("^C\n", STDOUT_FILENO);
	else if (signum == SIGQUIT)
		ft_putstr_fd("^\\Quit (core dumped)\n", STDOUT_FILENO);
}

void	setup_signal_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, handle_signal_child);
	signal(SIGQUIT, handle_signal_child);

}


typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIR_IN,
	TOKEN_REDIR_OUT,
	TOKEN_APPEND,
	TOKEN_HEREDOC
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	struct s_token	*next;
}	t_token;

typedef struct s_tokenizer
{
	char	*input;
	int		i;
	char	buffer[4096];
	int		j;
	t_token	*tokens;
}	t_tokenizer;

static int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

static t_token	*create_token(char *value, t_token_type type)
{
	t_token	*new_token;

	new_token = (t_token *)malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = ft_strdup(value);
	if (!new_token->value)
	{
		free(new_token);
		return (NULL);
	}
	new_token->type = type;
	new_token->next = NULL;
	return (new_token);
}

static void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}

static int	handle_quote(char *input, int *i, char quote, char *result)
{
	int	j;

	j = 0;
	(*i)++;
	while (input[*i] && input[*i] != quote)
	{
		result[j++] = input[*i];
		(*i)++;
	}
	if (!input[*i])
		return (-1);
	(*i)++;
	result[j] = '\0';
	return (0);
}

static void	handle_whitespace(t_tokenizer *tokenizer)
{
	if (tokenizer->j > 0)
	{
		tokenizer->buffer[tokenizer->j] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_WORD));
		tokenizer->j = 0;
	}
	tokenizer->i++;
}

static void	add_token_to_tokenizer(t_tokenizer *tokenizer, char *str, int type)
{
	add_token(&tokenizer->tokens, create_token(str, type));
}

static void	cleanup_tokens(t_tokenizer *tokenizer)
{
	t_token	*temp;

	while (tokenizer->tokens)
	{
		temp = tokenizer->tokens;
		tokenizer->tokens = tokenizer->tokens->next;
		free(temp->value);
		free(temp);
	}
}

static int	handle_quotes(t_tokenizer *tokenizer)
{
	char	quote;
	char	quoted_str[4096];

	if (tokenizer->j > 0)
	{
		tokenizer->buffer[tokenizer->j] = '\0';
		add_token_to_tokenizer(tokenizer, tokenizer->buffer, TOKEN_WORD);
		tokenizer->j = 0;
	}
	quote = tokenizer->input[tokenizer->i];
	ft_memset(quoted_str, 0, sizeof(quoted_str));
	if (handle_quote(tokenizer->input, &tokenizer->i, quote, quoted_str) == -1)
	{
		cleanup_tokens(tokenizer);
		return (-1);
	}
	add_token_to_tokenizer(tokenizer, quoted_str, TOKEN_WORD);
	return (0);
}

static void	handle_pipes(t_tokenizer *tokenizer)
{
	if (tokenizer->j > 0)
	{
		tokenizer->buffer[tokenizer->j] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_WORD));
		tokenizer->j = 0;
	}
	tokenizer->buffer[0] = '|';
	tokenizer->buffer[1] = '\0';
	add_token(&tokenizer->tokens, create_token(tokenizer->buffer, TOKEN_PIPE));
	tokenizer->i++;
}

static void	handle_double_redirections(t_tokenizer *tokenizer)
{
	if (tokenizer->input[tokenizer->i] == '<'
		&& tokenizer->input[tokenizer->i + 1] == '<')
	{
		tokenizer->buffer[0] = '<';
		tokenizer->buffer[1] = '<';
		tokenizer->buffer[2] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_HEREDOC));
		tokenizer->i += 2;
	}
	else if (tokenizer->input[tokenizer->i] == '>'
		&& tokenizer->input[tokenizer->i + 1] == '>')
	{
		tokenizer->buffer[0] = '>';
		tokenizer->buffer[1] = '>';
		tokenizer->buffer[2] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_APPEND));
		tokenizer->i += 2;
	}
}

static void	handle_single_redirections(t_tokenizer *tokenizer)
{
	if (tokenizer->input[tokenizer->i] == '<')
	{
		tokenizer->buffer[0] = '<';
		tokenizer->buffer[1] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_REDIR_IN));
		tokenizer->i++;
	}
	else if (tokenizer->input[tokenizer->i] == '>')
	{
		tokenizer->buffer[0] = '>';
		tokenizer->buffer[1] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_REDIR_OUT));
		tokenizer->i++;
	}
}

static void	handle_redirections(t_tokenizer *tokenizer)
{
	if (tokenizer->j > 0)
	{
		tokenizer->buffer[tokenizer->j] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_WORD));
		tokenizer->j = 0;
	}
	handle_double_redirections(tokenizer);
	handle_single_redirections(tokenizer);
}

static void	add_final_token(t_tokenizer *tokenizer)
{
	if (tokenizer->j > 0)
	{
		tokenizer->buffer[tokenizer->j] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_WORD));
	}
}

static void	handle_token(t_tokenizer *tokenizer)
{
	if (is_whitespace(tokenizer->input[tokenizer->i]))
		handle_whitespace(tokenizer);
	else if (tokenizer->input[tokenizer->i] == '\''
		|| tokenizer->input[tokenizer->i] == '"')
	{
		if (handle_quotes(tokenizer) == -1)
			return ;
	}
	else if (tokenizer->input[tokenizer->i] == '|')
		handle_pipes(tokenizer);
	else if (tokenizer->input[tokenizer->i] == '<'
		|| tokenizer->input[tokenizer->i] == '>')
		handle_redirections(tokenizer);
	else
		tokenizer->buffer[tokenizer->j++] = tokenizer->input[tokenizer->i++];
}

t_token	*tokenize_input(char *input)
{
	t_tokenizer tokenizer;

	tokenizer.input = input;
	tokenizer.i = 0;
	tokenizer.j = 0;
	tokenizer.tokens = NULL;

	while (tokenizer.input[tokenizer.i])
		handle_token(&tokenizer);
	add_final_token(&tokenizer);
	return (tokenizer.tokens);
}

t_commands	*init_command(void)
{
	t_commands	*cmd;

	cmd = (t_commands *)malloc(sizeof(t_commands));
	if (!cmd)
		return (NULL);
	cmd->cmd = NULL;
	cmd->args = NULL;
	cmd->argc = 0;
	cmd->type = 0;
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->input_fd = STDIN_FILENO;
	cmd->output_fd = STDOUT_FILENO;
	cmd->append_mode = 0;
	cmd->heredoc = 0;
	cmd->delimiter = NULL;
	cmd->g_pid = -1;
	cmd->next = NULL;
	cmd->prev = NULL;
	return (cmd);
}

static int	set_command_name(t_commands *cmd, char *arg)
{
	if (!cmd->cmd)
	{
		cmd->cmd = ft_strdup(arg);
		if (!cmd->cmd)
			return (-1);
	}
	return (0);
}

static int	add_to_arguments(t_commands *cmd, char *arg)
{
	char	**new_args;
	int		i;

	new_args = (char **)malloc(sizeof(char *) * (cmd->argc + 2));
	if (!new_args)
		return (-1);
	i = 0;
	while (i < cmd->argc)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[cmd->argc] = ft_strdup(arg);
	if (!new_args[cmd->argc])
	{
		free(new_args);
		return (-1);
	}
	new_args[cmd->argc + 1] = NULL;
	if (cmd->args)
		free(cmd->args);
	cmd->args = new_args;
	cmd->argc++;
	return (0);
}

static int	add_argument(t_commands *cmd, char *arg)
{
	if (set_command_name(cmd, arg) == -1)
		return (-1);
	if (add_to_arguments(cmd, arg) == -1)
		return (-1);
	return (0);
}

static void	free_tokens(t_token *tokens)
{
	t_token	*temp;

	while (tokens)
	{
		temp = tokens;
		tokens = tokens->next;
		if (temp->value)
			free(temp->value);
		free(temp);
	}
}

void	free_commands(t_commands *commands)
{
	t_commands	*temp;
	int			i;

	while (commands)
	{
		temp = commands;
		commands = commands->next;
		if (temp->cmd)
			free(temp->cmd);
		if (temp->args)
		{
			i = -1;
			while (++i < temp->argc)
				free(temp->args[i]);
			free(temp->args);
		}
		if (temp->input_file)
			free(temp->input_file);
		if (temp->output_file)
			free(temp->output_file);
		if (temp->delimiter)
			free(temp->delimiter);
		free(temp);
	}
}

static t_commands	*create_new_command(t_commands **cmd_list,
    t_commands **current_cmd)
{
	t_commands	*new_cmd;

	new_cmd = init_command();
	if (!new_cmd)
		return (NULL);
	if (!*cmd_list)
	{
		*cmd_list = new_cmd;
		*current_cmd = new_cmd;
		return (new_cmd);
	}
	(*current_cmd)->next = new_cmd;
	new_cmd->prev = *current_cmd;
	*current_cmd = new_cmd;
	return (new_cmd);
}

static int	handle_redirection(t_commands *current_cmd, t_token **current)
{
	if ((*current)->type == TOKEN_REDIR_IN)
	{
		if (!(*current)->next || (*current)->next->type != TOKEN_WORD)
			return (-1);
		if (current_cmd->input_file)
			free(current_cmd->input_file);
		current_cmd->input_file = ft_strdup((*current)->next->value);
		*current = (*current)->next;
		return (0);
	}
	if ((*current)->type == TOKEN_REDIR_OUT
		|| (*current)->type == TOKEN_APPEND)
	{
		if (!(*current)->next || (*current)->next->type != TOKEN_WORD)
			return (-1);
		if (current_cmd->output_file)
			free(current_cmd->output_file);
		current_cmd->output_file = ft_strdup((*current)->next->value);
		current_cmd->append_mode = ((*current)->type == TOKEN_APPEND);
		*current = (*current)->next;
		return (0);
	}
	return (0);
}

static int	handle_heredoc(t_commands *current_cmd, t_token **current)
{
	if ((*current)->type != TOKEN_HEREDOC)
		return (0);
	if (!(*current)->next || (*current)->next->type != TOKEN_WORD)
		return (-1);
	current_cmd->heredoc = 1;
	if (current_cmd->delimiter)
		free(current_cmd->delimiter);
	current_cmd->delimiter = ft_strdup((*current)->next->value);
	*current = (*current)->next;
	return (0);
}

static void	handle_error(t_commands **cmd_list)
{
	if (*cmd_list)
		free_commands(*cmd_list);
}

static int	initialize_command(t_commands **cmd_list, t_commands **current_cmd,
    t_token **current)
{
	if (!*cmd_list || (*current)->type == TOKEN_PIPE)
	{
		if ((*current)->type == TOKEN_PIPE)
			*current = (*current)->next;
		if (!create_new_command(cmd_list, current_cmd))
		{
			handle_error(cmd_list);
			return (-1);
		}
	}
	return (0);
}

static int	process_token(t_commands **cmd_list, t_commands **current_cmd,
    t_token **current)
{
	if ((*current)->type == TOKEN_WORD)
	{
		if (add_argument(*current_cmd, (*current)->value) < 0)
		{
			handle_error(cmd_list);
			return (-1);
		}
	}
	else if (handle_redirection(*current_cmd, current) < 0)
	{
		handle_error(cmd_list);
		return (-1);
	}
	else if (handle_heredoc(*current_cmd, current) < 0)
	{
		handle_error(cmd_list);
		return (-1);
	}
	return (0);
}

t_commands	*parse_tokens(t_token *tokens)
{
	t_commands	*cmd_list;
	t_commands	*current_cmd;
	t_token		*current;

	cmd_list = NULL;
	current = NULL;
	current = tokens;
	while (current)
	{
		if (initialize_command(&cmd_list, &current_cmd, &current) < 0)
			return (NULL);
		if (process_token(&cmd_list, &current_cmd, &current) < 0)
			return (NULL);
		current = current->next;
	}
	return (cmd_list);
}

t_commands	*parse_input(char *input)
{
	t_token		*tokens;
	t_commands	*commands;

	tokens = tokenize_input(input);
	if (!tokens)
		return (NULL);
	commands = parse_tokens(tokens);
	free_tokens(tokens);
	return (commands);
}

void print_commands(t_commands *cmd)
{
	int i;
	int cmd_num = 1;

	while (cmd)
	{
		printf("\n---- Command %d ----\n", cmd_num++);
		printf("Command: %s\n", cmd->cmd ? cmd->cmd : "(null)");
		printf("Arguments (%d): ", cmd->argc);
		for (i = 0; i < cmd->argc; i++)
            printf("[%s] ", cmd->args[i]);
        printf("\n");
        if (cmd->input_file)
            printf("Input redirection: %s\n", cmd->input_file);
        
        if (cmd->heredoc)
            printf("Heredoc with delimiter: %s\n", cmd->delimiter);
            
        if (cmd->output_file)
            printf("Output redirection: %s (mode: %s)\n", 
                cmd->output_file, cmd->append_mode ? "append" : "overwrite");
        if (cmd->next)
            printf("Piped to next command\n");
        cmd = cmd->next;
    }
    printf("\n");
}

typedef struct s_exec_cmd
{
	int		ori_in;
	int		ori_out;
	int		fdin;
	int		fdout;
	int		pipefd[2];
	pid_t	pid;
	int		status;
}	t_exec_cmd;

int	print_error(char *s, int exit)
{
	perror(s);
	return (exit);
}

void	free_path(char **paths)
{
	int	i;

	i = -1;
	while (paths[++i])
		free(paths[i]);
	free(paths);
}

char	*get_path(char *cmd, char **envp)
{
	char	*part_path;
	char	**paths;
	char	*path;
	int		i;

	i = 0;
	while (ft_strnstr(envp[i], "PATH=", 5) == NULL)
		i++;
	paths = ft_split(envp[i] + 5, ':');
	i = -1;
	while (paths[++i])
	{
		part_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(part_path, cmd);
		free(part_path);
		if (!path)
			break ;
		if (access(path, F_OK) == 0)
			return (path);
		free(path);
	}
	free_path(paths);
	return (NULL);
}

int setup_input(t_commands *cmd, t_exec_cmd *vars)
{
	if (cmd->input_file)
	{
		vars->fdin = open(cmd->input_file, O_RDONLY);
		if (vars->fdin < 0)
			return (print_error("open", -1));
	}
	else
		vars->fdin = dup(vars->ori_in);
	return (0);
}

int	handle_last_command_output(t_commands *cmd, t_exec_cmd *vars)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (cmd->append_mode)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	vars->fdout = open(cmd->output_file, flags, 0644);
	if (vars->fdout < 0)
		return (print_error("open", -1));
	return (0);
}

int	handle_piped_command_output(t_exec_cmd *vars)
{
	if (pipe(vars->pipefd) == -1) //pipe process
		return (print_error("pipe", -1));
	vars->fdout = vars->pipefd[1];
	vars->fdin = vars->pipefd[0];
	return (0);
}

int	setup_output(t_commands *cmd, t_exec_cmd *vars)
{
	//if its the last cmd, check to print at outfile or terminal
	if (cmd->next == NULL)
	{
		if (cmd->output_file)
		{
			if (handle_last_command_output(cmd, vars) < 0)
				return (-1);
		}
		else
			vars->fdout = dup(vars->ori_out);

	}
	else //if not pipe the output to the next cmd
		if (handle_piped_command_output(vars) < 0)
			return (-1);
	return (0);
}

int	execute_command(t_commands *cmd, t_exec_cmd *vars, char **envp, int *g_exit_status)
{
	char	*path;

	setup_signal_child();
	vars->pid = fork();
	if (vars->pid == 0)
	{
		path = get_path(cmd->cmd, envp);
		if (path == NULL)
		{
			printf("%s: command not found\n", cmd->cmd);
			exit(127);
		}
		execve(path, cmd->args, envp);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (vars->pid > 0) //Update global pid
		cmd->g_pid = vars->pid;
	else if (vars->pid < 0)
		return (print_error("fork", -1));
	return (0);
}

void	save_original_fd(t_exec_cmd *vars)
{
	vars->status = 0;
	vars->ori_in = dup(STDIN_FILENO);
	vars->ori_out = dup(STDOUT_FILENO);
}

void	restore_original_fd(t_exec_cmd *vars)
{
	dup2(vars->ori_in, STDIN_FILENO);
	dup2(vars->ori_out, STDOUT_FILENO);
	close(vars->ori_in);
	close(vars->ori_out);
}

void	set_exit_status(t_exec_cmd *vars, int *g_exit_status)
{
	if (WIFEXITED(vars->status))
		*g_exit_status = WEXITSTATUS(vars->status);
	else if (WIFSIGNALED(vars->status))
		*g_exit_status = 128 + WTERMSIG(vars->status);
	else
		*g_exit_status = 127;
}

void	print_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
}

void	print_pwd(char **envp)
{
	char	*pwd;

	pwd = ft_getenv("PWD", envp);
	printf("%s\n", pwd);
}

void	print_echo(char **commands)
{
	int	new_line;
	int	i;

	i = 1;
	new_line = 0;
	if (!commands[1])
		printf("\n");
	else if (ft_strcmp(commands[1], "-n") == 0)
	{	
		new_line = 1;
		i = 2;
	}
	while(commands[i])
	{
		printf("%s", commands[i]);
		if (commands[i + 1] != NULL)
			printf(" ");
		i++;
	}
	if (new_line == 0)
		printf("\n");
}

static int	is_valid_variable_name(const char *name)
{
	if (!name || !*name)
		return (0);
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	while (*++name)
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
	return (1);
}

static int	find_variable(char *var_name, char **envp)
{
	int		i;
	size_t	len;

	i = 0;
	len = ft_strlen(var_name);
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, len) == 0 && envp[i][len] == '=')
			return (i);
		i++;
	}
	return (-1);
}

static void	add_new_variable(char ***envp, char *new_var)
{
	int		count;
	char	**new_envp;
	int		i;

	count = 0;
	while ((*envp)[count])
		count++;
	new_envp = (char **)ft_calloc(count + 2, sizeof(char *));
	if (!new_envp)
	{
		free(new_var);
		return ;
	}
	i = 0;
	while (i < count)
	{
		new_envp[i] = (*envp)[i];
		i++;
	}
	new_envp[count] = new_var;
	new_envp[count + 1] = NULL;
	free(*envp);
	*envp = new_envp;
}

static void	set_variable(char *var_name, char *var_value, char ***envp)
{
	int		i;
	char	*tmp;
	char	*new_var;

	i = find_variable(var_name, *envp);
	if (*var_value == '\0')
		tmp = ft_strdup(var_name);
	else
		tmp = ft_strjoin(var_name, "=");
	new_var = ft_strjoin(tmp, var_value);
	free(tmp);
	if (!new_var)
		return ;
	if (i >= 0)
	{
		free((*envp)[i]);
		(*envp)[i] = new_var;
	}
	else
		add_new_variable(envp, new_var);
}

char	**copy_envp(char **envp)
{
	char	**copy;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	copy = (char **)malloc(sizeof(char *) * (i + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

static void	sort_envp(char **envp)
{
	int		count;
	int		i;
	int		j;
	char	*temp;

	count = 0;
	i = 0;
	while (envp[count])
		count++;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(envp[j], envp[j + 1]) > 0)
			{
				temp = envp[j];
				envp[j] = envp[j + 1];
				envp[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

static void	print_exported_variables(char **envp)
{
	int	i;

	sort_envp(envp);
	i = 0;
	while (envp[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(envp[i], STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		i++;
	}
}

static void	handle_export_argument(char *arg, char ***envp)
{
	char	*equal;

	equal = ft_strchr(arg, '=');
	if (equal)
		*equal = '\0';
	if (!is_valid_variable_name(arg))
	{
		if (equal)
			*equal = '=';
		ft_putstr_fd("export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': not an identifier\n", STDERR_FILENO);
	}
	else if (equal)
	{
		set_variable(arg, equal + 1, envp);
		*equal = '=';
	}
	else if (find_variable(arg, *envp) == -1)
		set_variable(arg, "", envp);
}

void	export_variable(char **args, char ***envp)
{
	int	i;

	if (!args[1])
	{
		print_exported_variables(*envp);
		return ;
	}
	i = 1;
	while (args[i])
	{
		handle_export_argument(args[i], envp);
		i++;
	}
}

static int	find_index(char *var, char **envp)
{
	int		i;
	size_t	len;

	len = ft_strlen(var);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var, len) == 0
			&& (envp[i][len] == '=' || envp[i][len] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

void	unset_variable(char *var, char ***envp)
{
	int	index;
	int	j;

	if (!var)
	{
		ft_putstr_fd("unset: missing argument\n", 2);
		return ;
	}
	index = find_index(var, *envp);
	if (index >= 0)
	{
		free((*envp)[index]);
		j = index;
		while ((*envp)[j])
		{
			(*envp)[j] = (*envp)[j + 1];
			j++;
		}
	}
}

char	*ft_strcpy(char *dest, char *src)
{
	int	i;

	i = -1;
	while (src[++i])
		dest[i] = src[i];
	dest[i] = '\0';
	return (dest);
}

char	*ft_strcat(char *dest, char *src)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (dest[i])
		i++;
	while (src[j])
		dest[i++] = src[j++];
	dest[i] = '\0';
	return (dest);
}

void	update_env(char *dir, char *name, char ****mini_envp)
{
	char	*new;
	int		i;

	i = 0;
	new = malloc(sizeof(char) * (ft_strlen(dir) + ft_strlen(name) + 2));
	if (!new)
		return ;
	ft_strcpy(new, name);
	ft_strcat(new, "=");
	ft_strcat(new, dir);
	while ((**mini_envp)[i])
	{
		if (ft_strncmp((**mini_envp)[i], name, ft_strlen(name)) == 0)
		{
			free((**mini_envp)[i]);
			(**mini_envp)[i] = new;
			return ;
		}
		i++;
	}
}

void	change_directory(t_commands **commands, char ***mini_envp, int *g_exit_status)
{
	char	**cmd;
	char	*home;
	char	cur_dir[4096];
	char	new_dir[4096];

	if ((*commands)->next != NULL)
	{
		*commands = (*commands)->next;
		return ;
	}
	getcwd(cur_dir, 4096);
	cmd = (*commands)->args;
	if (!cmd[1])
	{
		home = ft_getenv("HOME", *mini_envp);
		chdir(home);
	}
	else if (ft_strcmp(cmd[1], "..") == 0)
		chdir("..");
	else if (ft_strcmp(cmd[1], "-") == 0)
	{
		home = ft_getenv("OLDPWD", *mini_envp);
		printf("%s\n", ft_getenv("OLDPWD", *mini_envp));
		chdir(home);
	}
	else
	{
		if (chdir(cmd[1]) != 0)
		{
			ft_putstr_fd("bash: cd: ", STDERR_FILENO);
            ft_putstr_fd(cmd[1], STDERR_FILENO);
            ft_putstr_fd(": ", STDERR_FILENO);
            perror("");
			*g_exit_status = 1;
		}
	}
	getcwd(new_dir, 4096);
	update_env(cur_dir, "OLDPWD", &mini_envp);
	update_env(new_dir, "PWD", &mini_envp);
}

void	print_exit_status(int *g_exit_status)
{
	printf("%d\n", *g_exit_status);
	*g_exit_status = 0;
}

void execute_commands(t_commands *cmd_list, char **envp, int *g_exit_status)
{
	t_exec_cmd	vars;

	save_original_fd(&vars);
	if (setup_input(cmd_list, &vars) < 0)
		return ;
	while (cmd_list)
	{
		dup2(vars.fdin, STDIN_FILENO);
		close(vars.fdin);
		if (setup_output(cmd_list, &vars) < 0)
			return ;
		dup2(vars.fdout, STDOUT_FILENO);
		close(vars.fdout);
		if (strcmp(cmd_list->cmd, "echo") == 0 && cmd_list->args[1]
			&& strcmp(cmd_list->args[1], "$?") == 0)
			print_exit_status(g_exit_status);
		else if (strcmp(cmd_list->cmd, "cd") == 0)
			change_directory(&cmd_list, &envp, g_exit_status);
		else if (strcmp(cmd_list->cmd, "echo") == 0)
			print_echo(cmd_list->args);
		else if (strcmp(cmd_list->cmd, "env") == 0)
			print_env(envp);
		else if (strcmp(cmd_list->cmd, "pwd") == 0)
			print_pwd(envp);
		else if (execute_command(cmd_list, &vars, envp, g_exit_status) < 0)
			return ;
		cmd_list = cmd_list->next;
	}
	restore_original_fd(&vars);
	waitpid(vars.pid, &vars.status, 0);
	if (vars.status)
		set_exit_status(&vars, g_exit_status);
}

void	exit_program(t_commands *commands, char **mini_envp, int *g_exit_status)
{
	free_commands(commands);
	free_path(mini_envp);
	exit (*g_exit_status);
}

int	is_num(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (!(s[i] >= 48 && s[i] <= 57))
			return (0);
		i++;
	}
	return (1);
}

void	unset_env(t_commands *commands, char ** mini_envp)
{
	int	i;

	i = 1;

	while (commands->args[i])
	{
		unset_variable(commands->args[i], &mini_envp);
		i++;
	}
}

void	check_exit_value(t_commands *commands, int *g_exit_status)
{
	int	temp;

	if (is_num(commands->args[1]))
	{
		temp = ft_atoi(commands->args[1]);
		*g_exit_status = (temp % 256);
	}
	else
	{
		ft_putstr_fd("bash: exit: ", STDERR_FILENO);
		ft_putstr_fd(commands->args[1], STDERR_FILENO);
		ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
		*g_exit_status = 2;
	}
}

void	execution(t_commands *commands, char **mini_envp, int *g_exit_status)
{
	if (strcmp(commands->cmd, "export") == 0)
		export_variable(commands->args, &mini_envp);
	else if (strcmp(commands->cmd, "unset") == 0)
		unset_env(commands, mini_envp);
	else if (strcmp(commands->cmd, "exit") == 0)
	{
		ft_putstr_fd("exit\n", STDERR_FILENO);
		if (commands->args[1])
			check_exit_value(commands, g_exit_status);
		exit_program(commands, mini_envp, g_exit_status);
	}
	else
		execute_commands(commands, mini_envp, g_exit_status);
}

int main(int argc, char **argv, char **envp)
{
	char			**mini_envp;
	char			*input;
	t_commands		*commands;
	struct termios	original_term;
	struct termios	new_term;
	int				g_exit_status;

	g_exit_status = 0;
	mini_envp = copy_envp(envp);
	while (1)
	{
		setup_signal_handlers(&original_term, &new_term);
		input = readline("minishell> ");
		if (g_signal == 130)
		{
			g_exit_status = g_signal;
			g_signal = 0;
		}
		if (!input)
		{
			printf("exit\n");
			break ;
		}
		if (*input)
			add_history(input);
		commands = parse_input(input);
		if (commands)
		{
			execution(commands, mini_envp, &g_exit_status);
			free_commands(commands);
		}
		else
			printf("\n");
		free(input);
	}
	tcsetattr(STDERR_FILENO, TCSANOW, &original_term);
	free_path(mini_envp);
	rl_clear_history();
	return (g_exit_status);
}
