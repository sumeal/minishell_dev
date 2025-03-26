/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
<<<<<<< HEAD
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:49:25 by jchen2            #+#    #+#             */
/*   Updated: 2025/03/25 08:28:03 by abin-moh         ###   ########.fr       */
=======
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/25 17:49:25 by jchen2            #+#    #+#             */
/*   Updated: 2025/03/24 14:57:13 by jchen2           ###   ########.fr       */
>>>>>>> refs/remotes/origin/main
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <sys/stat.h>
# include "libft.h"

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
}	t_cmd;

<<<<<<< HEAD
=======

>>>>>>> refs/remotes/origin/main
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



#endif