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
	home = getenv("HOME");
	/*
	// Crashes under Ubuntu at env[8].
	char *envar, *value, envi[1024];
	int i = 0;
	while (env[i])
	{
		printf("i: %d\n", i);
		printf("env[i]: %s\n", env[i]);
		// backup envi
		strcpy(envi, env[i]);

		// extract envvar
		envar = strtok(env[i], "=");
		printf("Envar: %s, ", envar);
		value = strtok(0, "=");
		printf("Value: %s\n", value);

		// check if have found the home path...
		if (strcmp(envar, "HOME") == 0)
		{
			printf("Found Home!\n");
			strcpy(home, value);
			strcpy(env[i], envi);
			break;
		}
		strcpy(env[i], envi);
		i++;
	}*/
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
	processPipe(line, env);
	/*
	for (i = 0; i < j; i++)
	{
		processPipe(commands[i]);
	}*/
}

void processPipe(char *pipe, char *env[])
{
	char pipeArray[512], *cmd, *arg, argArray[512][512];

	strcpy(pipeArray, pipe);

	cmd = strtok(pipeArray, " ");

	if (strcmp(cmd, "exit") != 0 && strcmp(cmd, "cd") != 0)
	{
		int pid, status, i = 1;
		strcpy(argArray[0], cmd);
		while (arg = strtok(0, " "))
		{
			strcpy(argArray[i], arg);
			i++;
		}
		argArray[i+1] = NULL;
		printf("\n");

		// TODO: fork process for the command!
		if ((pid = fork()) == 0)
		{
			execvp(argArray[0], argArray);
			exit(1);
		}
		else
		{
			pid = wair(&status);
			printf("Child Exit Code: %d\n", pid);
		}
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
