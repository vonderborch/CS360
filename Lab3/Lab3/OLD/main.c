#include "common.h"

char line[512], *home;

int main(int argc, char *argv[], char *env[])
{
	home = getenv("HOME");

	while (1)
	{
		// variables
		int pid, status;
		char *cmd, *arg, argArray[512][512];

		// reset line
		memset(line, 0, sizeof(line));

		// get input
		printf("Command? ");
		scanf(" %[^\n]", line);

		// process command
		cmd = strtok(line, " ");
		// cd command
		if (strcmp(cmd, "cd") == 0)
		{
			arg = strtok(0, " ");
			// no directory specified
			if (!arg)
			{
				if (home)
				{
					chdir(home);
				}
			}
			// directory specified
			else
				chdir(arg);
		}
		else if (strcmp(cmd, "pwd") == 0)
			printf("%s\n", getcwd());
		// exit command
		else if (strcmp(cmd, "exit") == 0)
			exit(1);
		// other command
		else
		{
			pid = fork();
			if (pid == 0)
			{
				execvp(cmd, cmd);
			}
			else
			{
				wait(pid);
			}
		}
	}

	return 0;
}