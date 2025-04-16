/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:03:21 by abin-moh          #+#    #+#             */
/*   Updated: 2025/04/16 15:14:14 by abin-moh         ###   ########.fr       */
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
# include <stdio.h>
# include <stdbool.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <dirent.h>
# include <errno.h>
# include <sys/stat.h>

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
	int		**pipefd;
	pid_t	*pid;
	int		status;
	int		builtin_executed;
	int		cmd_count;
	int		i;
}	t_exec_cmd;

typedef struct s_bool
{
	bool	w_s;
	bool	s_s;
	bool	d_s;
}	t_bool;

typedef enum e_token_type
{
	CMD_ARGS,
	PIPE,
	REDIR_IN,
	REDIR_OUT,
	APPEND,
	HEREDOC
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	struct s_token	*next;
}	t_token;

typedef struct s_cmd
{
	char			**argv;
	char			*input_file;
	char			*output_file;
	int				append;
	char			*hd_delimeter;
	int				is_hd;
	struct s_cmd	*next;
	struct s_cmd	*prev;
}	t_cmd;

//free function
void	free_lexems(t_token *lexems);
void	free_arr(char **arr);
void	free_cmds(t_cmd *cmd);

// lexical analysis
char	*count_copy_substr(char **str, t_bool *flag);
void	count_substr(char **str, int *sublen, t_bool *flag);
char	**ft_split_quotes(char *str);
char	**split_keep_delimiter(char const *s, char c);
char	*expand_lexem(char *s, char **mini_envp, int status);
char	*expand_str(char *s, char **mini_envp, int status);
t_token	*lexical_analysis(char *input, char **mini_envp, int status);

// syntactic analysis
t_cmd	*syntactic_analysis(char *input, char **mini_envp, int *status);
int		is_valid_syntax(t_token *lexems);
int		add_arg_to_cmd(t_cmd *cmd, char *value);
int		handle_hd_apd(t_cmd *cmd, t_token **lexem);
int		handle_redir(t_cmd *cmd, t_token **lexem);

/*execution.c*/
void	create_child_processes(t_cmd *cmd, t_exec_cmd *vars,
			int *g_exit_status, char ***envp);
void	execute_multiple_commands(t_cmd **cmd,
			char ***envp, int *g_exit_status);
void	execution(t_cmd *commands, char ***mini_envp, int *g_exit_status);
int		execute_builtin_command(t_cmd **cmd_list,
			char ***envp, int *g_exit_status);

/*execution2.c*/
void	setup_io(t_cmd *cmd, int cmd_count,
			t_exec_cmd *vars, int *g_exit_status);
void	execute_external_command(t_cmd *cur, char **envp);
void	wait_for_child(t_exec_cmd *vars);
void	execute_command_in_child(t_cmd *cur,
			t_exec_cmd *vars, int *g_exit_status, char ***envp);

/*export.c*/
int		export_variable(char **args, char ***envp, int *g_exit_status);
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
int		unset_env(t_cmd *commands, char **mini_envp, int *g_exit_status);
void	unset_variable(char *var, char ***envp);
int		find_index(char *var, char **envp);

/*exit.c*/
void	exit_program(t_cmd *commands,
			char **mini_envp, int *g_exit_status);
int		is_num(char *s);
int		check_exit_value(t_cmd *commands,
			char **mini_envp, int *g_exit_status);
void	error_too_many_arg(int *g_exit_status);
void	error_numeric_arg(char *cmd, int *g_exit_status);

/*execution_utils.c*/
void	save_original_fd(t_exec_cmd *vars);
void	restore_original_fd(t_exec_cmd *vars);
void	set_exit_status(t_exec_cmd *vars, int *g_exit_status);

/*execution_utils2.c*/
int		print_error(char *s, int exit);
void	free_path(char **paths);
char	*get_path(char *cmd, char **envp);
int		setup_input(t_cmd *cmd, t_exec_cmd *vars, int *g_exit_status);
int		handle_last_command_output(t_cmd *cmd, t_exec_cmd *vars);

/*execution_utils3.c*/
void	handle_signal_heredoc(int signum);
int		hd_printf(char *hd_delimeter);
int		exit_function(t_cmd *commands, char **mini_envp, int *g_exit_status);
void	handle_input_redir(t_cmd *cmd, t_exec_cmd *vars, int *g_exit_status);
void	handle_output_redir(t_cmd *cmd, int cmd_count, t_exec_cmd *vars);

/*execution_utils4.c*/
void	free_pipe(t_exec_cmd *vars);
char	*get_shlvl_value(char **envp, int index);
void	increment_shlvl(char ***envp);

/*execution_builtin.c*/
int		print_echo(char **commands, int *g_exit_status);
void	handle_directory_change(char **cmd,
			char ***mini_envp, int *g_exit_status);
void	update_env_vars(char **mini_envp, char *cur_dir);
void	change_directory(t_cmd **commands,
			char ***mini_envp, int *g_exit_status);

/*execution_builtin2.c*/
void	print_error_cd(char *failed_cmd, int *g_exit_status);
void	change_to_pwd(char ***mini_envp, int *g_exit_status);
void	update_env(char *dir, char *name, char ***mini_envp);
int		print_env(t_cmd **cmd_list, char **envp, int *g_exit_status);
int		print_pwd(char **envp, int *g_exit_status);

/*execution_builtin3.c*/
char	*ft_getenv(char *name, char **envp);
int		ignore_export(int *g_exit_status);
int		count_command(t_cmd *cmd);
void	closing_pipes(t_exec_cmd **vars);

/*signal.c*/
void	handle_signal_parent(int signum);
void	setup_signal_handlers(struct termios *original_term,
			struct termios *new_term);
void	handle_signal_child(int signum);
void	setup_signal_child(void);
void	setup_signal_heredoc(void);

/*libft_utils.c*/
char	*ft_strcpy(char *dest, char *src);
char	*ft_strcat(char *dest, char *src);
int		ft_strcmp(const char *s1, const char *s2);

#endif