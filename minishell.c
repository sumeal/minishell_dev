/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:12:22 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/26 14:04:54 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t g_signal = 0;

// typedef enum e_token_type
// {
// 	TOKEN_WORD,
// 	TOKEN_PIPE,
// 	TOKEN_REDIR_IN,
// 	TOKEN_REDIR_OUT,
// 	TOKEN_APPEND,
// 	TOKEN_HEREDOC
// }	t_token_type;

// typedef struct s_token
// {
// 	char			*value;
// 	t_token_type	type;
// 	struct s_token	*next;
// }	t_token;

// typedef struct s_tokenizer
// {
// 	char	*input;
// 	int		i;
// 	char	buffer[4096];
// 	int		j;
// 	t_token	*tokens;
// }	t_tokenizer;

// static int	is_whitespace(char c)
// {
// 	return (c == ' ' || c == '\t' || c == '\n');
// }

// static t_token	*create_token(char *value, t_token_type type)
// {
// 	t_token	*new_token;

// 	new_token = (t_token *)malloc(sizeof(t_token));
// 	if (!new_token)
// 		return (NULL);
// 	new_token->value = ft_strdup(value);
// 	if (!new_token->value)
// 	{
// 		free(new_token);
// 		return (NULL);
// 	}
// 	new_token->type = type;
// 	new_token->next = NULL;
// 	return (new_token);
// }

// static void	add_token(t_token **head, t_token *new_token)
// {
// 	t_token	*current;

// 	if (!*head)
// 	{
// 		*head = new_token;
// 		return ;
// 	}
// 	current = *head;
// 	while (current->next)
// 		current = current->next;
// 	current->next = new_token;
// }

// static int	handle_quote(char *input, int *i, char quote, char *result)
// {
// 	int	j;

// 	j = 0;
// 	(*i)++;
// 	while (input[*i] && input[*i] != quote)
// 	{
// 		result[j++] = input[*i];
// 		(*i)++;
// 	}
// 	if (!input[*i])
// 		return (-1);
// 	(*i)++;
// 	result[j] = '\0';
// 	return (0);
// }

// static void	handle_whitespace(t_tokenizer *tokenizer)
// {
// 	if (tokenizer->j > 0)
// 	{
// 		tokenizer->buffer[tokenizer->j] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_WORD));
// 		tokenizer->j = 0;
// 	}
// 	tokenizer->i++;
// }

// static void	add_token_to_tokenizer(t_tokenizer *tokenizer, char *str, int type)
// {
// 	add_token(&tokenizer->tokens, create_token(str, type));
// }

// static void	cleanup_tokens(t_tokenizer *tokenizer)
// {
// 	t_token	*temp;

// 	while (tokenizer->tokens)
// 	{
// 		temp = tokenizer->tokens;
// 		tokenizer->tokens = tokenizer->tokens->next;
// 		free(temp->value);
// 		free(temp);
// 	}
// }

// static int	handle_quotes(t_tokenizer *tokenizer)
// {
// 	char	quote;
// 	char	quoted_str[4096];

// 	if (tokenizer->j > 0)
// 	{
// 		tokenizer->buffer[tokenizer->j] = '\0';
// 		add_token_to_tokenizer(tokenizer, tokenizer->buffer, TOKEN_WORD);
// 		tokenizer->j = 0;
// 	}
// 	quote = tokenizer->input[tokenizer->i];
// 	ft_memset(quoted_str, 0, sizeof(quoted_str));
// 	if (handle_quote(tokenizer->input, &tokenizer->i, quote, quoted_str) == -1)
// 	{
// 		cleanup_tokens(tokenizer);
// 		return (-1);
// 	}
// 	add_token_to_tokenizer(tokenizer, quoted_str, TOKEN_WORD);
// 	return (0);
// }

// static void	handle_pipes(t_tokenizer *tokenizer)
// {
// 	if (tokenizer->j > 0)
// 	{
// 		tokenizer->buffer[tokenizer->j] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_WORD));
// 		tokenizer->j = 0;
// 	}
// 	tokenizer->buffer[0] = '|';
// 	tokenizer->buffer[1] = '\0';
// 	add_token(&tokenizer->tokens, create_token(tokenizer->buffer, TOKEN_PIPE));
// 	tokenizer->i++;
// }

// static void	handle_double_redirections(t_tokenizer *tokenizer)
// {
// 	if (tokenizer->input[tokenizer->i] == '<'
// 		&& tokenizer->input[tokenizer->i + 1] == '<')
// 	{
// 		tokenizer->buffer[0] = '<';
// 		tokenizer->buffer[1] = '<';
// 		tokenizer->buffer[2] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_HEREDOC));
// 		tokenizer->i += 2;
// 	}
// 	else if (tokenizer->input[tokenizer->i] == '>'
// 		&& tokenizer->input[tokenizer->i + 1] == '>')
// 	{
// 		tokenizer->buffer[0] = '>';
// 		tokenizer->buffer[1] = '>';
// 		tokenizer->buffer[2] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_APPEND));
// 		tokenizer->i += 2;
// 	}
// }

// static void	handle_single_redirections(t_tokenizer *tokenizer)
// {
// 	if (tokenizer->input[tokenizer->i] == '<')
// 	{
// 		tokenizer->buffer[0] = '<';
// 		tokenizer->buffer[1] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_REDIR_IN));
// 		tokenizer->i++;
// 	}
// 	else if (tokenizer->input[tokenizer->i] == '>')
// 	{
// 		tokenizer->buffer[0] = '>';
// 		tokenizer->buffer[1] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_REDIR_OUT));
// 		tokenizer->i++;
// 	}
// }

// static void	handle_redirections(t_tokenizer *tokenizer)
// {
// 	if (tokenizer->j > 0)
// 	{
// 		tokenizer->buffer[tokenizer->j] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_WORD));
// 		tokenizer->j = 0;
// 	}
// 	handle_double_redirections(tokenizer);
// 	handle_single_redirections(tokenizer);
// }

// static void	add_final_token(t_tokenizer *tokenizer)
// {
// 	if (tokenizer->j > 0)
// 	{
// 		tokenizer->buffer[tokenizer->j] = '\0';
// 		add_token(&tokenizer->tokens,
// 			create_token(tokenizer->buffer, TOKEN_WORD));
// 	}
// }

// static void	handle_token(t_tokenizer *tokenizer)
// {
// 	if (is_whitespace(tokenizer->input[tokenizer->i]))
// 		handle_whitespace(tokenizer);
// 	else if (tokenizer->input[tokenizer->i] == '\''
// 		|| tokenizer->input[tokenizer->i] == '"')
// 	{
// 		if (handle_quotes(tokenizer) == -1)
// 			return ;
// 	}
// 	else if (tokenizer->input[tokenizer->i] == '|')
// 		handle_pipes(tokenizer);
// 	else if (tokenizer->input[tokenizer->i] == '<'
// 		|| tokenizer->input[tokenizer->i] == '>')
// 		handle_redirections(tokenizer);
// 	else
// 		tokenizer->buffer[tokenizer->j++] = tokenizer->input[tokenizer->i++];
// }

// t_token	*tokenize_input(char *input)
// {
// 	t_tokenizer	tokenizer;

// 	tokenizer.input = input;
// 	tokenizer.i = 0;
// 	tokenizer.j = 0;
// 	tokenizer.tokens = NULL;

// 	while (tokenizer.input[tokenizer.i])
// 		handle_token(&tokenizer);
// 	add_final_token(&tokenizer);
// 	return (tokenizer.tokens);
// }

// t_commands	*init_command(void)
// {
// 	t_commands	*cmd;

// 	cmd = (t_commands *)malloc(sizeof(t_commands));
// 	if (!cmd)
// 		return (NULL);
// 	cmd->cmd = NULL;
// 	cmd->args = NULL;
// 	cmd->argc = 0;
// 	cmd->type = 0;
// 	cmd->input_file = NULL;
// 	cmd->output_file = NULL;
// 	cmd->input_fd = STDIN_FILENO;
// 	cmd->output_fd = STDOUT_FILENO;
// 	cmd->append_mode = 0;
// 	cmd->heredoc = 0;
// 	cmd->delimiter = NULL;
// 	cmd->next = NULL;
// 	cmd->prev = NULL;
// 	return (cmd);
// }

// static int	set_command_name(t_commands *cmd, char *arg)
// {
// 	if (!cmd->cmd)
// 	{
// 		cmd->cmd = ft_strdup(arg);
// 		if (!cmd->cmd)
// 			return (-1);
// 	}
// 	return (0);
// }

// static int	add_to_arguments(t_commands *cmd, char *arg)
// {
// 	char	**new_args;
// 	int		i;

// 	new_args = (char **)malloc(sizeof(char *) * (cmd->argc + 2));
// 	if (!new_args)
// 		return (-1);
// 	i = 0;
// 	while (i < cmd->argc)
// 	{
// 		new_args[i] = cmd->args[i];
// 		i++;
// 	}
// 	new_args[cmd->argc] = ft_strdup(arg);
// 	if (!new_args[cmd->argc])
// 	{
// 		free(new_args);
// 		return (-1);
// 	}
// 	new_args[cmd->argc + 1] = NULL;
// 	if (cmd->args)
// 		free(cmd->args);
// 	cmd->args = new_args;
// 	cmd->argc++;
// 	return (0);
// }

// static int	add_argument(t_commands *cmd, char *arg)
// {
// 	if (set_command_name(cmd, arg) == -1)
// 		return (-1);
// 	if (add_to_arguments(cmd, arg) == -1)
// 		return (-1);
// 	return (0);
// }

// static void	free_tokens(t_token *tokens)
// {
// 	t_token	*temp;

// 	while (tokens)
// 	{
// 		temp = tokens;
// 		tokens = tokens->next;
// 		if (temp->value)
// 			free(temp->value);
// 		free(temp);
// 	}
// }

// void	free_commands(t_commands *commands)
// {
// 	t_commands	*temp;
// 	int			i;

// 	while (commands)
// 	{
// 		temp = commands;
// 		commands = commands->next;
// 		if (temp->cmd)
// 			free(temp->cmd);
// 		if (temp->args)
// 		{
// 			i = -1;
// 			while (++i < temp->argc)
// 				free(temp->args[i]);
// 			free(temp->args);
// 		}
// 		if (temp->input_file)
// 			free(temp->input_file);
// 		if (temp->output_file)
// 			free(temp->output_file);
// 		if (temp->delimiter)
// 			free(temp->delimiter);
// 		free(temp);
// 	}
// }

// static t_commands	*create_new_command(t_commands **cmd_list,
//     t_commands **current_cmd)
// {
// 	t_commands	*new_cmd;

// 	new_cmd = init_command();
// 	if (!new_cmd)
// 		return (NULL);
// 	if (!*cmd_list)
// 	{
// 		*cmd_list = new_cmd;
// 		*current_cmd = new_cmd;
// 		return (new_cmd);
// 	}
// 	(*current_cmd)->next = new_cmd;
// 	new_cmd->prev = *current_cmd;
// 	*current_cmd = new_cmd;
// 	return (new_cmd);
// }

// static int	handle_redirection(t_commands *current_cmd, t_token **current)
// {
// 	if ((*current)->type == TOKEN_REDIR_IN)
// 	{
// 		if (!(*current)->next || (*current)->next->type != TOKEN_WORD)
// 			return (-1);
// 		if (current_cmd->input_file)
// 			free(current_cmd->input_file);
// 		current_cmd->input_file = ft_strdup((*current)->next->value);
// 		*current = (*current)->next;
// 		return (0);
// 	}
// 	if ((*current)->type == TOKEN_REDIR_OUT
// 		|| (*current)->type == TOKEN_APPEND)
// 	{
// 		if (!(*current)->next || (*current)->next->type != TOKEN_WORD)
// 			return (-1);
// 		if (current_cmd->output_file)
// 			free(current_cmd->output_file);
// 		current_cmd->output_file = ft_strdup((*current)->next->value);
// 		current_cmd->append_mode = ((*current)->type == TOKEN_APPEND);
// 		*current = (*current)->next;
// 		return (0);
// 	}
// 	return (0);
// }

// static int	handle_heredoc(t_commands *current_cmd, t_token **current)
// {
// 	if ((*current)->type != TOKEN_HEREDOC)
// 		return (0);
// 	if (!(*current)->next || (*current)->next->type != TOKEN_WORD)
// 		return (-1);
// 	current_cmd->heredoc = 1;
// 	if (current_cmd->delimiter)
// 		free(current_cmd->delimiter);
// 	current_cmd->delimiter = ft_strdup((*current)->next->value);
// 	*current = (*current)->next;
// 	return (0);
// }

// static void	handle_error(t_commands **cmd_list)
// {
// 	if (*cmd_list)
// 		free_commands(*cmd_list);
// }

// static int	initialize_command(t_commands **cmd_list, t_commands **current_cmd,
//     t_token **current)
// {
// 	if (!*cmd_list || (*current)->type == TOKEN_PIPE)
// 	{
// 		if ((*current)->type == TOKEN_PIPE)
// 		{
// 			if (!(*current = (*current)->next))
// 				handle_error(cmd_list);
// 				return (-1);
// 		}
// 		if (!create_new_command(cmd_list, current_cmd))
// 		{
// 			handle_error(cmd_list);
// 			return (-1);
// 		}
// 	}
// 	return (0);
// }

// static int	process_token(t_commands **cmd_list, t_commands **current_cmd,
//     t_token **current)
// {
// 	if ((*current)->type == TOKEN_WORD)
// 	{
// 		if (add_argument(*current_cmd, (*current)->value) < 0)
// 		{
// 			handle_error(cmd_list);
// 			return (-1);
// 		}
// 	}
// 	else if (handle_redirection(*current_cmd, current) < 0)
// 	{
// 		handle_error(cmd_list);
// 		return (-1);
// 	}
// 	else if (handle_heredoc(*current_cmd, current) < 0)
// 	{
// 		handle_error(cmd_list);
// 		return (-1);
// 	}
// 	return (0);
// }

// t_commands	*parse_tokens(t_token *tokens)
// {
// 	t_commands	*cmd_list;
// 	t_commands	*current_cmd;
// 	t_token		*current;

// 	cmd_list = NULL;
// 	current = NULL;
// 	current = tokens;
// 	while (current)
// 	{
// 		if (initialize_command(&cmd_list, &current_cmd, &current) < 0)
// 			return (NULL);
// 		if (process_token(&cmd_list, &current_cmd, &current) < 0)
// 			return (NULL);
// 		current = current->next;
// 	}
// 	return (cmd_list);
// }

// t_commands	*parse_input(char *input)
// {
// 	t_token		*tokens;
// 	t_commands	*commands;

// 	tokens = tokenize_input(input);
// 	if (!tokens)
// 		return (NULL);
// 	commands = parse_tokens(tokens);
// 	free_tokens(tokens);
// 	return (commands);
// }

// void print_parsed_commands(t_commands *cmd_list)
// {
//     int i;

//     printf("Parsed Commands:\n");
//     while (cmd_list)
//     {
//         printf("Command: %s\n", cmd_list->cmd ? cmd_list->cmd : "(null)");
//         printf("Arguments:\n");
//         for (i = 0; i < cmd_list->argc; i++)
//         {
//             printf("  [%d]: %s\n", i, cmd_list->args[i]);
//         }
//         printf("Input File: %s\n", cmd_list->input_file ? cmd_list->input_file : "(none)");
//         printf("Output File: %s\n", cmd_list->output_file ? cmd_list->output_file : "(none)");
//         printf("Append Mode: %s\n", cmd_list->append_mode ? "Yes" : "No");
//         printf("Heredoc: %s\n", cmd_list->heredoc ? "Yes" : "No");
//         printf("Delimiter: %s\n", cmd_list->delimiter ? cmd_list->delimiter : "(none)");
//         printf("Type: %d\n", cmd_list->type);
//         printf("Next Command: %s\n", cmd_list->next ? cmd_list->next->cmd : "(none)");
//         printf("Previous Command: %s\n", cmd_list->prev ? cmd_list->prev->cmd : "(none)");
//         printf("-----------------------------\n");
//         cmd_list = cmd_list->next;
//     }
// }

void print_cmd(t_cmd *cmd)
{
	int i;
	
	printf("Command:\n");
	
	// Print args
	if (cmd->argv)
	{
		i = 0;
		printf("  Arguments: ");
		while (cmd->argv[i])
		{
			printf("\"%s\" ", cmd->argv[i]);
			i++;
		}
		printf("\n");
	}
	else
		printf("  Arguments: None\n");
	
	// Print redirections
	if (cmd->input_file)
		printf("  Input file: %s\n", cmd->input_file);
	if (cmd->output_file)
	{
		if (cmd->append)
			printf("  Output file (append): %s\n", cmd->output_file);
		else
			printf("  Output file: %s\n", cmd->output_file);
	}
	if (cmd->is_hd)
		printf("  Heredoc delimiter: %s\n", cmd->hd_delimeter);
}

char	**copy_envp(char **envp)
{
	char	**copy;
	int		i;

	i = count_envp(envp);
	copy = (char **)malloc(sizeof(char *) * (i + 1));
	if (!copy)
		return (NULL);
	i = -1;
	while (envp[++i])
		copy[i] = ft_strdup(envp[i]);
	copy[i] = NULL;
	return (copy);
}

int main(int argc, char **argv, char **envp)
{
	char			**mini_envp;
	char			*input;
	t_cmd			*commands;
	struct termios	original_term;
	struct termios	new_term;
	int				g_exit_status;

	g_signal = 0;
	(void)argc;
	(void)argv;
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
		commands = syntactic_analysis(input, mini_envp, &g_exit_status);
		if (commands)
		{
			while(commands)
			{
				print_cmd(commands);
				commands = commands->next;
			}
			//execution(commands, &mini_envp, &g_exit_status);
			free_cmds(commands);
		}
		free(input);
	}
	tcsetattr(STDERR_FILENO, TCSANOW, &original_term);
	free_path(mini_envp);
	rl_clear_history();
	return (g_exit_status);
}
