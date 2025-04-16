/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:12:22 by abin-moh          #+#    #+#             */
/*   Updated: 2025/04/16 16:20:20 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

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

void	free_all(char **mini_envp, struct termios *original_term)
{
	tcsetattr(STDERR_FILENO, TCSANOW, original_term);
	free_path(mini_envp);
	rl_clear_history();
}


void print_cmd(t_cmd *cmd)
{
	int i;

	if (!cmd)
	{
		printf("🔍 Command is NULL\n");
		return;
	}

	printf("🧾 Command Details:\n");

	// Print arguments
	printf("  argv: ");
	if (cmd->argv)
	{
		for (i = 0; cmd->argv[i]; i++)
			printf("\"%s\" ", cmd->argv[i]);
		printf("\n");
	}
	else
		printf("(null)\n");

	// Print redirections
	printf("  input_file: %s\n", cmd->input_file ? cmd->input_file : "(null)");
	printf("  output_file: %s\n", cmd->output_file ? cmd->output_file : "(null)");
	printf("  append: %d\n", cmd->append);
	printf("  here_doc delimeter: %s\n", cmd->hd_delimeter ? cmd->hd_delimeter : "(null)");
	printf("  is_here_doc: %d\n", cmd->is_hd);

	// Optional: show links in the command list
	printf("  has next: %s\n", cmd->next ? "yes" : "no");
	printf("  has prev: %s\n", cmd->prev ? "yes" : "no");
	printf("--------------------------------\n");
}


void	minishell_loop(char ***mini_envp, struct termios *original_term,
			struct termios *new_term, int *g_exit_status)
{
	char	*input;
	t_cmd	*commands;

	while (1)
	{
		setup_signal_handlers(original_term, new_term);
		input = readline("🚀minishell🚀> ");
		if (g_signal == 130)
		{
			*g_exit_status = g_signal;
			g_signal = 0;
		}
		if (!input)
			break ;
		if (*input)
			add_history(input);
		commands = syntactic_analysis(input, *mini_envp, g_exit_status);
		if (commands)
		{
			while (commands != NULL)
			{
				print_cmd(commands);
				commands = commands->next;
			}
			//execution(commands, mini_envp, g_exit_status);
			free_cmds(commands);
		}
		free(input);
	}
	printf("exit\n");
}

char	**init_env(char **envp)
{
	char	**mini_envp;

	mini_envp = copy_envp(envp);
	increment_shlvl(&mini_envp);
	return (mini_envp);
}


int	main(int argc, char **argv, char **envp)
{
	char			**mini_envp;
	struct termios	original_term;
	struct termios	new_term;
	int				g_exit_status;

	(void)argc;
	(void)argv;
	g_exit_status = 0;
	mini_envp = init_env(envp);
	minishell_loop(&mini_envp, &original_term, &new_term, &g_exit_status);
	free_all(mini_envp, &original_term);
	return (g_exit_status);
}
