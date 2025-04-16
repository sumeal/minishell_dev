/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_valid_syntax.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abin-moh <abin-moh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 17:16:27 by jchen2            #+#    #+#             */
/*   Updated: 2025/04/15 15:12:52 by abin-moh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	redir_error(t_token *next)
{
	char	*symbol;

	symbol = "";
	if (!next)
	{
		ft_putstr_fd("minishell: syntax error ", 2);
		ft_putstr_fd("near unexpected token `newline'\n", 2);
		return (2);
	}
	if (next->type == REDIR_IN)
		symbol = "<";
	else if (next->type == REDIR_OUT)
		symbol = ">";
	else if (next->type == APPEND)
		symbol = ">>";
	else if (next->type == HEREDOC)
		symbol = "<<";
	else if (next->type == PIPE)
		symbol = "|";
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(symbol, 2);
	ft_putstr_fd("'\n", 2);
	return (2);
}

static int	pipe_error(t_token *next)
{
	char	*symbol;

	symbol = "";
	if (next->type == PIPE)
	{
		symbol = "|";
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(symbol, 2);
		ft_putstr_fd("'\n", 2);
	}
	else
	{
		ft_putstr_fd("minishell: syntax error ", 2);
		ft_putstr_fd("near unexpected token `newline'\n", 2);
	}
	return (2);
}

static int	syntax_error(t_token *lexems)
{
	t_token	*next;

	while (lexems)
	{
		next = lexems->next;
		if ((lexems->type == PIPE && next && next->type != CMD_ARGS))
			return (pipe_error(next));
		if ((lexems->type == REDIR_IN || lexems->type == REDIR_OUT || 
			lexems->type == APPEND || lexems->type == HEREDOC))
		{
			if (!next || next->type != CMD_ARGS)
				return (redir_error(next));
		}
		if (next == NULL && lexems->type == PIPE)
		{
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
			return (2);
		}
		lexems = lexems->next;
	}
	return (0);
}

int	is_valid_syntax(t_token *lexems)
{
	if (!lexems)
		return (0);
	if (lexems->type == PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (2);
	}
	return (syntax_error(lexems));
}
