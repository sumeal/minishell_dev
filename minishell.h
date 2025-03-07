/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/28 12:03:21 by abin-moh          #+#    #+#             */
/*   Updated: 2025/02/28 12:10:47 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

typedef struct s_commands
{
	char            *cmd;           // Command string
	char            **args;         // Array of command arguments
	int             argc;           // Number of arguments
	int             type;           // Command type identifier
	char            *input_file;    // Input redirection file
	char            *output_file;   // Output redirection file
	int             input_fd;       // Input file descriptor
	int             output_fd;      // Output file descriptor 
	int             append_mode;    // Flag for append redirection (>>)
	int             heredoc;        // Flag for heredoc (<<)
	char            *delimiter;     // Heredoc delimiter
	struct s_commands *next;        // Pointer to next command in pipeline
	struct s_commands *prev;        // Pointer to previous command
}	t_commands

#endif