NAME = minishell

FILES = execution_builtin.c \
		execution_builtin2.c \
		execution_builtin3.c \
		execution_utils.c \
		execution_utils2.c \
		execution_utils3.c \
		execution.c \
		exit.c \
		export.c \
		export2.c \
		export3.c \
		libft_utils.c \
		minishell.c \
		signal.c \
		unset.c \
		expand_lexem.c \
		expand_utils_one.c \
		expand_utils_two.c \
		free_funcs.c \
		ft_split_quotes.c \
		ft_split_utils_one.c \
		ft_split_utils_two.c \
		is_valid_syntax.c \
		lecical_analysis.c \
		syntactic_analysis_utils.c \
		syntactic_analysis.c

LIBFT = libft.a

OFILES = $(FILES:.c=.o)

CC = cc 
CFLAGS = -Wall -Wextra -Werror

$(NAME): $(OFILES)
	$(CC) $(CFLAGS) $(FILES) $(LIBFT) -lreadline -o $(NAME)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(NAME)

clean:
	rm -f $(OFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
