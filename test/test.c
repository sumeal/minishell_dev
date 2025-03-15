#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *home;

	home = getenv("HOME");
	printf("%s\n", home);
	return (0);
}