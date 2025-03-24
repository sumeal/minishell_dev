#include "minishell.h"

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n');
}

t_token	*create_token(char *value, t_token_type type)
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

void	add_token(t_token **head, t_token *new_token)
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

int	handle_quote(char *input, int *i, char quote, char *result)
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

void	handle_whitespace(t_tokenizer *tokenizer)
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
void	add_token_to_tokenizer(t_tokenizer *tokenizer, char *str, int type)
{
	add_token(&tokenizer->tokens, create_token(str, type));
}
void	cleanup_tokens(t_tokenizer *tokenizer)
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

int	handle_quotes(t_tokenizer *tokenizer)
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

void	handle_pipes(t_tokenizer *tokenizer)
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

void	handle_double_redirections(t_tokenizer *tokenizer)
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

void	handle_single_redirections(t_tokenizer *tokenizer)
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

void	handle_redirections(t_tokenizer *tokenizer)
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

void	add_final_token(t_tokenizer *tokenizer)
{
	if (tokenizer->j > 0)
	{
		tokenizer->buffer[tokenizer->j] = '\0';
		add_token(&tokenizer->tokens,
			create_token(tokenizer->buffer, TOKEN_WORD));
	}
}

void	handle_token(t_tokenizer *tokenizer)
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

int	set_command_name(t_commands *cmd, char *arg)
{
	if (!cmd->cmd)
	{
		cmd->cmd = ft_strdup(arg);
		if (!cmd->cmd)
			return (-1);
	}
	return (0);
}

int	add_to_arguments(t_commands *cmd, char *arg)
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

int	add_argument(t_commands *cmd, char *arg)
{
	if (set_command_name(cmd, arg) == -1)
		return (-1);
	if (add_to_arguments(cmd, arg) == -1)
		return (-1);
	return (0);
}

void	free_tokens(t_token *tokens)
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

t_commands	*create_new_command(t_commands **cmd_list,
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

int	handle_redirection(t_commands *current_cmd, t_token **current)
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

int	handle_heredoc(t_commands *current_cmd, t_token **current)
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

void	handle_error(t_commands **cmd_list)
{
	if (*cmd_list)
		free_commands(*cmd_list);
}

int	initialize_command(t_commands **cmd_list, t_commands **current_cmd,
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

int	process_token(t_commands **cmd_list, t_commands **current_cmd,
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
