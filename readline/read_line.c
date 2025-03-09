#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <signal.h>

void	sigint_handler(int signum)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	sigquit_handler(int signum)
{
	write(1, "\nexit\n", 6);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	abort();
}

int main()
{
	char *line;
	struct sigaction sa_int;
	struct sigaction sa_quit;

	sa_int.sa_handler = sigint_handler;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = sigquit_handler;
	sa_quit.sa_flags = 0;
	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGQUIT, &sa_quit, NULL);

	while (1)
	{
		line = readline("minis > ");
		if (!line)
		{
			printf("exit\n");
			break ;
		}
		if (line)
			add_history(line);
		printf("%s\n", line);
		free(line);
	}
	rl_clear_history();
	return (0);
}
