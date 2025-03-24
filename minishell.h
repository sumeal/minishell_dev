/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:03:21 by abin-moh          #+#    #+#             */
/*   Updated: 2025/03/24 15:26:17 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <signal.h>
# include <termios.h>

extern volatile sig_atomic_t	g_signal;

typedef struct s_mini_envp
{
	char				*name;
	char				*equal;
	char				*value;
	struct s_mini_envp	*next;
}	t_mini_envp;

typedef struct s_exec_cmd
{
	int		ori_in;
	int		ori_out;
	int		fdin;
	int		fdout;
	int		pipefd[2];
	pid_t	pid;
	int		status;
	int		builtin_executed;
}	t_exec_cmd;

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
/*                                                          DELETE these 3 struct later*/
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

/*execution.c*/
void	execution(t_commands *commands, char ***mini_envp, int *g_exit_status);
void	execute_commands(t_commands *cmd_list, char **envp, int *g_exit_status);
int		execute_command(t_commands *cmd,
			t_exec_cmd *vars, char **envp, int *g_exit_status);
int		execute_external_command(t_commands *cmd_list,
			t_exec_cmd *vars, char **envp, int *g_exit_status);
int		execute_builtin_command(t_commands **cmd_list,
			char ***envp, int *g_exit_status);

/*export.c*/
void	export_variable(char **args, char ***envp, int *g_exit_status);
void	print_sorted_envp(char **envp);
char	**duplicate_env_array(char **envp);
void	sort_env(char **envp);
void	swap_envp(char **envp, int i, int j);

/*export2.c*/
int		count_envp(char **envp);
char	*get_var_name(char *envp);
int		add_variable_to_env(char ***envp, char **args);
int		find_variable(char *args, char **envp);
int		check_valid_value(char *s);

/*export3.c*/
void	add_new_variable(char ***envp, char **new_var);
char	*change_format(char *args);
int		skip(char *envp1, char *envp2);

/*unset.c*/
void	unset_env(t_commands *commands, char **mini_envp, int *g_exit_status);
void	unset_variable(char *var, char ***envp);
int		find_index(char *var, char **envp);

/*exit.c*/
void	exit_program(t_commands *commands,
			char **mini_envp, int *g_exit_status);
int		is_num(char *s);
void	check_exit_value(t_commands *commands, int *g_exit_status);
void	free_commands(t_commands *commands);

/*execution_utils.c*/
void	save_original_fd(t_exec_cmd *vars);
void	restore_original_fd(t_exec_cmd *vars);
void	set_exit_status(t_exec_cmd *vars, int *g_exit_status);
int		handle_piped_command_output(t_exec_cmd *vars);
int		setup_output(t_commands *cmd, t_exec_cmd *vars);

/*execution_utils2.c*/
int		print_error(char *s, int exit);
void	free_path(char **paths);
char	*get_path(char *cmd, char **envp);
int		setup_input(t_commands *cmd, t_exec_cmd *vars);
int		handle_last_command_output(t_commands *cmd, t_exec_cmd *vars);

/*execution_builtin.c*/
int		print_echo(char **commands, int *g_exit_status);
int		print_exit_status(int *g_exit_status);
void	handle_directory_change(char **cmd,
			char ***mini_envp, int *g_exit_status);
void	update_env_vars(char **mini_envp, char *cur_dir);
void	change_directory(t_commands **commands,
			char ***mini_envp, int *g_exit_status);

/*execution_builtin2.c*/
void	print_error_cd(char *failed_cmd, int *g_exit_status);
void	change_to_pwd(char ***mini_envp, int *g_exit_status);
void	update_env(char *dir, char *name, char ***mini_envp);
int		print_env(t_commands **cmd_list, char **envp, int *g_exit_status);
int		print_pwd(char **envp, int *g_exit_status);

/*execution_builtin3.c*/
char	*ft_getenv(char *name, char **envp);

/*signal.c*/
void	handle_signal_parent(int signum);
void	setup_signal_handlers(struct termios *original_term,
			struct termios *new_term);
void	handle_signal_child(int signum);
void	setup_signal_child(void);

/*libft_utils.c*/
char	*ft_strcpy(char *dest, char *src);
char	*ft_strcat(char *dest, char *src);
int		ft_strcmp(const char *s1, const char *s2);

/*parse_input.c                                                                                    DELETE LATER */
int	is_whitespace(char c);
t_token	*create_token(char *value, t_token_type type);
void	add_token(t_token **head, t_token *new_token);
int	handle_quote(char *input, int *i, char quote, char *result);
void	handle_whitespace(t_tokenizer *tokenizer);
void	add_token_to_tokenizer(t_tokenizer *tokenizer, char *str, int type);
void	cleanup_tokens(t_tokenizer *tokenizer);
int	handle_quotes(t_tokenizer *tokenizer);
void	handle_pipes(t_tokenizer *tokenizer);
void	handle_double_redirections(t_tokenizer *tokenizer);
void	handle_single_redirections(t_tokenizer *tokenizer);
void	handle_redirections(t_tokenizer *tokenizer);
void	add_final_token(t_tokenizer *tokenizer);
void	handle_token(t_tokenizer *tokenizer);
t_token	*tokenize_input(char *input);
t_commands	*init_command(void);
int	set_command_name(t_commands *cmd, char *arg);
int	add_to_arguments(t_commands *cmd, char *arg);
int	add_argument(t_commands *cmd, char *arg);
void	free_tokens(t_token *tokens);
t_commands	*create_new_command(t_commands **cmd_list,
    t_commands **current_cmd);
int	handle_redirection(t_commands *current_cmd, t_token **current);
int	handle_heredoc(t_commands *current_cmd, t_token **current);
void	handle_error(t_commands **cmd_list);
int	initialize_command(t_commands **cmd_list, t_commands **current_cmd,
    t_token **current);
int	process_token(t_commands **cmd_list, t_commands **current_cmd,
    t_token **current);
t_commands	*parse_tokens(t_token *tokens);
t_commands	*parse_input(char *input);

#endif