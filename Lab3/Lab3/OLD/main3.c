#include "common.h"

char line[512], *home;

int main(int argc, char *argv[], char *env[])
{
	// Setup basic variables
	int isRunning = 1;

	findHomeDirectory(env);

	while (isRunning)
	{
		char *cmd, *arg;
		// reset line
		memset(line, 0, sizeof(line));

		// get input
		printf("Command? ");
		scanf(" %[^\n]", line);

		commandProcessor(env);
	}

	return 0;
}

void findHomeDirectory(char *env[])
{
	char *envar, *value, envi[1024];
	int i = 0;
	while (env[i])
	{
		// backup envi
		strcpy(envi, env[i]);

		// extract envvar
		envar = strtok(env[i], "=");
		value = strtok(0, "=");

		// check if have found the home path...
		if (strcmp(envar, "HOME") == 0)
		{
			strcpy(home, value);
			strcpy(env[i], envi);
			break;
		}
		strcpy(env[i], envi);
		i++;
	}
}

void commandProcessor(char *env[])
{
	int i = 0, j = 0;
	char *pipecmd, processingLine[512], commands[512][512];
	strcpy(processingLine, line);

	// create pipe command array
	pipecmd = strtok(processingLine, "|");
	while (pipecmd)
	{
		strcpy(commands[j], pipecmd);
		j++;
		pipecmd = strtok(0, "|");
	}

	// process the pipes
	processPipe(line);
	/*
	for (i = 0; i < j; i++)
	{
		processPipe(commands[i]);
	}*/
}

void processPipe(char *pipe, char *env[])
{
	char pipeArray[512], *cmd, *arg;

	strcpy(pipeArray, pipe);

	cmd = strtok(pipeArray, " ");

	if (strcmp(cmd, "exit") != 0 && strcmp(cmd, "cd") != 0)
	{
		printf("%s: ", cmd);
		while (arg = strtok(0, " "))
		{
			printf("%s, ", arg);
			// TODO: build up an arg array
		}
		printf("\n");

		// TODO: fork process for the command!
		// TODO: wait for process to die!
		// TODO: print process exit code!
	}
	// "Special" commands
	else
	{
		// cd command
		if (strcmp(cmd, "cd") == 0)
		{
			arg = strtok(0, " ");
			printf("cd: ");

			// no argument, default to home
			if (arg == NULL)
			{
				printf("changing directory to home...\n");
				if (home)
					chdir(home);
			}
			// argument, change to specified directory
			else
			{
				printf("changing to directory!\n");
				arg = strtok(0, " ");
				chdir(arg);
			}
		}
		// exit command
		else
			exit(1);
	}
}
