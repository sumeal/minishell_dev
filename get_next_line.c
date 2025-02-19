/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchen2 <jchen2@student.42kl.edu.my>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 19:12:02 by jchen2            #+#    #+#             */
/*   Updated: 2025/02/18 11:14:46 by jchen2           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*get_line(char **storage)
{
	char	*line;
	char	*temp;
	int		len;

	len = 0;
	temp = *storage;
	while ((*storage)[len] != '\n' && (*storage)[len] != '\0')
		len++;
	if ((*storage)[len] == '\n')
		len++;
	*storage = ft_strdup(*storage + len);
	if (!*storage)
	{
		free(temp);
		return (NULL);
	}
	line = (char *)malloc((len + 1) * sizeof(char));
	if (!line)
		return (NULL);
	line[len] = '\0';
	while (len--)
		line[len] = temp[len];
	free(temp);
	return (line);
}

static int	read_update(int fd, char **storage, char *block)
{
	ssize_t	count;
	char	*temp;

	while (1)
	{
		count = read(fd, block, BUFFER_SIZE);
		if (count <= 0)
			return (count);
		block[count] = '\0';
		if (*storage)
		{
			temp = *storage;
			*storage = ft_strjoin(*storage, block);
			free(temp);
		}
		else
			*storage = ft_strdup(block);
		if (!(*storage))
			return (-1);
		if (ft_strchr(*storage, '\n'))
			break ;
	}
	return (count);
}

char	*get_next_line(int fd)
{
	static char	*storage;
	char		*line;
	char		*block;
	int			count;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	block = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!block)
		return (NULL);
	count = read_update(fd, &storage, block);
	free(block);
	block = NULL;
	if (!storage)
		return (NULL);
	if (count == 0 && ft_strlen(storage) == 0)
	{
		free(storage);
		storage = NULL;
		return (NULL);
	}
	line = get_line(&storage);
	return (line);
}

#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("example.txt", O_RDONLY);
	line = get_next_line(fd);
	printf("%s\n", line);
	free(line);
	close(fd);
	return (0);
}
