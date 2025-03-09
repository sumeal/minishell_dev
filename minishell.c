/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:12:22 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/09 15:31:02 by abin-moh         ###   ########.fr       */
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

volatile sig_atomic_t g_signal = 0;

static void	handle_signal(int sig_num)
{
	g_signal = sig_num;
	if (sig_num == SIGINT)
	{
		ft_putstr_fd("\n", STDOUT_FILENO);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else if (sig_num == SIGQUIT)
		ft_putstr_fd("Quit: (core dumped)\n", STDOUT_FILENO);
}

void	sigint_handler(int signum)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	sigquit_handler(int signum)
{
	if (g_signal == 1)
		write(1, "Quit: (core dumped)\n", 20);
}

static void	setup_signal_handlers(void)
{
	// struct sigaction	sa;

	// sa.sa_handler = handle_signal;
	// sigemptyset(&sa.sa_mask);
	// sa.sa_flags = SA_RESTART;

	// if (sigaction(SIGINT, &sa, NULL) == -1)
	// {
	// 	ft_putstr_fd("Error setting up SIGINT handler\n", STDERR_FILENO);
	// }
	// if (sigaction(SIGQUIT, &sa, NULL) == -1)
	// {
	// 	ft_putstr_fd("Error setting up SIGQUIT handler\n", STDERR_FILENO);
	// }
	struct sigaction sa_int;
	struct sigaction sa_quit;

	sa_int.sa_handler = sigint_handler;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = sigquit_handler;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);
}

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
	struct s_commands	*next;
	struct s_commands	*prev;
}	t_commands;

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
	if (pipe(vars->pipefd) == -1)
		return (print_error("pipe", -1));
	vars->fdout = vars->pipefd[1];
	vars->fdin = vars->pipefd[0];
	return (0);
}

int	setup_output(t_commands *cmd, t_exec_cmd *vars)
{
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
	else
		if (handle_piped_command_output(vars) < 0)
			return (-1);
	return (0);
}

int	execute_command(t_commands *cmd, t_exec_cmd *vars, char **envp)
{
	char	*path;

	vars->pid = fork();
	if (vars->pid == 0)
	{
		path = get_path(cmd->cmd, envp);
		if (path == NULL)
		{
			perror("command not found");
			exit(EXIT_FAILURE);
		}
		execve(path, cmd->args, envp);
		perror("execve");
		exit(EXIT_FAILURE);
	}
	else if (vars->pid < 0)
		return (print_error("fork", -1));
	return (0);
}

void execute_commands(t_commands *cmd_list, char **envp)
{
	t_exec_cmd	vars;

	vars.ori_in = dup(STDIN_FILENO);
	vars.ori_out = dup(STDOUT_FILENO);
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
		if (execute_command(cmd_list, &vars, envp) < 0)
			return ;
		cmd_list = cmd_list->next;
	}
	dup2(vars.ori_in, STDIN_FILENO);
	dup2(vars.ori_out, STDOUT_FILENO);
	close(vars.ori_in);
	close(vars.ori_out);
	waitpid(vars.pid, &vars.status, 0);
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

static int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
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

int main(int argc, char **argv, char **envp)
{
    char **mini_envp;
    char *input;
    t_commands *commands;

    setup_signal_handlers();
    mini_envp = copy_envp(envp);

    while (1)
    {
        if (g_signal)
            g_signal = 0;
        input = readline("minishell> ");
        if (!input)
        {
            printf("exit\n");
            break;
        }
        if (*input)
            add_history(input);
        commands = parse_input(input);
        if (commands)
        {
            g_signal = 1;
			if (strcmp(commands->cmd, "export") == 0)
                export_variable(commands->args, &mini_envp);
            else if (strcmp(commands->cmd, "unset") == 0)
            {
                for (int i = 1; commands->args[i]; i++)
                {
                    unset_variable(commands->args[i], &mini_envp);
                }
            }
            else
                execute_commands(commands, mini_envp);
            free_commands(commands);
        }
        else
        {
            printf("Error parsing command\n");
        }
        free(input);
    }
    // Free the copied environment variables
	for (int i = 0; mini_envp[i]; i++)
        free(mini_envp[i]);
    free(mini_envp);
	rl_clear_history();
    return 0;
}
