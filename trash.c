static int	is_valid_variable_name(const char *name)
{
	if (!name || !*name)
		return (0);
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	while (*++name)
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
	return (1);
}