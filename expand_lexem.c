/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_lexem.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/14 10:15:18 by jchen2            #+#    #+#             */
/*   Updated: 2025/03/18 11:10:34 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
1. duplicate the input and checking the expansion situation
2. delete quotes if the input has quotes
3. final_word to get the value of lexem
*/

#include "minishell.h"

char	*remove_quotes(char *tmp)
{
	int		len;
	int		i;
	int		j;
	char	*str;

	len = ft_strlen(tmp);
	str = malloc(len - 1);
	if (!str)
		return (NULL);
	i = 1;
	j = 0;
	while (i < len - 1)
	{
		str[j] = tmp[i];
		i++;
		j++;
	}
	str[j] = '\0';
	return (str);
}

int	final_word(char *tmp, char **word)
{
	if (tmp[0] == '\'' || tmp[0] == '\"')
	{
		*word = remove_quotes(tmp);
		if (!*word)
		{
			free(tmp);
			return (0);
		}
		free(tmp);
	}
	else
		*word = tmp;
	return (1);
}

char	*expand_lexem(char *s, char **mini_envp, int status)
{
	char	*tmp;
	char	*word;
	char	*substr;

	substr = ft_strdup(s);
	if (!substr)
		return (NULL);
	if (substr[0] != '\'' && ft_strchr(substr, '$'))
	{
		tmp = expand_str(substr, mini_envp, status);
		if (!tmp)
		{
			free(substr);
			return (NULL);
		}
		free(substr);
	}
	else
		tmp = substr;
	if (final_word(tmp, &word) == 0)
		return (NULL);
	return (word);
}

/*
#include <stdio.h>
#include <stdlib.h>

void test_expand_lexem(char *input, char **mini_envp)
{
    char *expanded = expand_lexem(input, mini_envp);
    if (!expanded)
        printf("expand_lexem(%s) -> ERROR\n", input);
    else
    {
        printf("expand_lexem(%s) -> %s\n", input, expanded);
        free(expanded);
    }
}

int main(void)
{
    // 模拟环境变量数组
    char *mini_envp[] = {
        "USER=student",
        "HOME=/home/student",
        "SHELL=/bin/bash",
        "EMPTY=",
        NULL
    };

    printf("=== Testing expand_lexem ===\n");

    test_expand_lexem("hello", mini_envp);                 // 普通字符串
    test_expand_lexem("$USER", mini_envp);                 // 变量替换
    test_expand_lexem("$HOME", mini_envp);                 // 变量替换
    test_expand_lexem("$UNDEFINED_VAR", mini_envp);        // 未定义变量
    test_expand_lexem("Hello \"$USER!\"", mini_envp);          // 变量在字符串中
    test_expand_lexem("$EMPTY", mini_envp);                // 空变量
    test_expand_lexem("'$USER'", mini_envp);               // 单引号包裹，不展开
    test_expand_lexem("\"$USER\"", mini_envp);             // 双引号包裹，展开
    test_expand_lexem("This is $HOME and $SHELL", mini_envp); // 多个变量

    return 0;
}
*/