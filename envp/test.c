

























































char	**copy_envp(char **envp)
{
	char	**copy;
	int		i;

	i = 0;
	while (envp[i])
		i++;
	copy = (char **)malloc(sizeof(char *) * (i + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (envp[i])
	{
		copy[i] = ft_strdup(envp[i]);
		i++;
	}
	free(copy[i]);
	copy[i] = NULL;
	return (copy);
}

static void	print_exported_variables(char **envp)
{
	int		i;
	char	**sorted_envp;

	sorted_envp = envp;
	sort_envp(sorted_envp);
	i = 0;
	while (sorted_envp[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(sorted_envp[i], STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
		i++;
	}
	i = 0;
	while (sorted_envp[i])
	{
		free(sorted_envp[i]);
		i++;
	}
	free(sorted_envp);
}

static void	sort_envp(char **envp)
{
	int		count;
	int		i;
	int		j;
	char	*temp;

	count = 0;
	i = 0;
	while (envp[count] != NULL)
		count++;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(envp[j], envp[j + 1]) > 0)
			{
				temp = envp[j];
				envp[j] = envp[j + 1];
				envp[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

static void	handle_export_argument(char *arg, char ***envp)
{
	char	*equal;

	equal = ft_strchr(arg, '=');
	if (equal)
		*equal = '\0';
	if (!is_valid_variable_name(arg))
	{
		if (equal)
			*equal = '=';
		ft_putstr_fd("export: `", STDERR_FILENO);
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd("': not an identifier\n", STDERR_FILENO);
	}
	else if (equal)
	{
		set_variable(arg, equal + 1, envp);
		*equal = '=';
	}
	else if (find_variable(arg, *envp) == -1)
		set_variable(arg, "", envp);
}


void	export_variable(char **args, char ***envp, int *g_exit_status)
{
	int	i;

	if (!args[1])
	{
		print_exported_variables(*envp);
		return ;
	}
	i = 1;
	while (args[i])
	{
		handle_export_argument(args[i], envp);
		i++;
	}
	*g_exit_status = 0;
}