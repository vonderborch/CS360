#include "common.h"

typedef struct node {
	char name[64];
	char type;
	struct node *rchildPtr, *childPtr, *siblingPtr, *parentPtr;
} NODE;

NODE *root, *cwd;
char line[128];
char command[64], pathname[64];
char dirname[64], basename[64];

int main(void)
{
	// Setup Variables
	int isRunning = 1, cid = 0, good;
	good = initialize();
	if (good == 1)
	{
		printf("Root initialized!\n");
	}
	else
	{
		printf("Root not initialized, not enough space!\n");
		isRunning = 0;
	}

	printf("Enter 'help' (without quotes) to return list of commands.\n");


	while (isRunning == 1)
	{
		// get input
		printf("Command? ");
		scanf(" %[^\n]", line);

		// break up command into parts
		sscanf(line, "%s %s", command, pathname);

		// figure out command
		cid = findCommand(command);
		switch(cid)
		{
		case -1:
			// unknown token
			invalid();
			break;
		case 0: 
			// Display help
			help();
			break;
		case 1:
			// mkdir
			mkdir();
			break;
		case 2:
			// rmdir
			rmdir();
			break;
		case 3:
			// cd
			cd();
			break;
		case 4:
			// ls
			ls();
			break;
		case 5:
			// pwd
			pwd();
			break;
		case 6:
			// creat
			creat();
			break;
		case 7:
			// rm
			rm();
			break;
		case 8:
			// save
			save();
			break;
		case 9:
			// reload
			reload();
			break;
		case 10:
			// exit
			isRunning = 0;
			break;
		}

	}

	return 0;
}

// loading!
int initialize()
{
	Node *nNew = NULL;
	nNew = (NODE *) malloc (sizeof (NODE));
	if (nNew != NULL)
	{
		nNew->name = "";
		nNew->type = "D";
		nNew->rchildPtr = NULL;
		nNew->childPtr = NULL;
		nNew->siblingPtr = NULL;
		nNew->parentPtr = NULL;
		*root = nNew;
		*cwd = root;
		return 1;
	}
	return 0;
}

// what to do when an invalid command is present
void invalid()
{
	printf("Invalid command, please enter a valid command or enter 'help' for help!\n");
}

// what to do when the user asks for help
void help()
{
	printf("Possible Commands:\n help  mkdir  rmdir  cd  ls  pwd  creat  rm  save  reload  quit\n");
}

// what to do when the user asks to mkdir
void mkdir()
{
	if (hasPathName(pathname) == 0)
	{
		printf("Invalid format, please specify a pathname!\n");
	}
	else
	{
		breakupPathName();
	}
}

// what to do when the user asks to rmdir
void rmdir()
{
	if (hasPathName(pathname) == 0)
	{
		printf("Invalid format, please specify a pathname!\n");
	}
	else
	{
		breakupPathName();
	}
}

// what to do when the user asks to cd
void cd()
{
}

// what to do when the user asks to ls
void ls()
{
}

// what to do when the user asks to pwd
void pwd()
{
}

// what to do when the user asks to creat
void creat()
{
	if (hasPathName(pathname) == 0)
	{
		printf("Invalid format, please specify a pathname!\n");
	}
	else
	{
		breakupPathName();
	}
}

// what to do when the user asks to rm
void rm()
{
	if (hasPathName(pathname) == 0)
	{
		printf("Invalid format, please specify a pathname!\n");
	}
	else
	{
		breakupPathName();
	}
}

// what to do when the user asks to save
void save()
{
	if (hasPathName(pathname) == 0)
	{
		printf("Invalid format, please specify a filename!\n");
	}
	else
	{
		breakupPathName();
	}
}

// what to do when the user asks to reload
void reload()
{
	if (hasPathName(pathname) == 0)
	{
		printf("Invalid format, please specify a filename!\n");
	}
	else
	{
		breakupPathName();
	}
}

int findCommand(char command[64])
{
	if (strcmp(command,"help") == 0)
	{
		return 0;
	}
	else if (strcmp(command,"mkdir") == 0)
	{
		return 1;
	}
	else if (strcmp(command,"rmdir") == 0)
	{
		return 2;
	}
	else if (strcmp(command,"cd") == 0)
	{
		return 3;
	}
	else if (strcmp(command,"ls") == 0)
	{
		return 4;
	}
	else if (strcmp(command,"pwd") == 0)
	{
		return 5;
	}
	else if (strcmp(command,"creat") == 0)
	{
		return 6;
	}
	else if (strcmp(command,"rm") == 0)
	{
		return 7;
	}
	else if (strcmp(command,"save") == 0)
	{
		return 8;
	}
	else if (strcmp(command,"reload") == 0)
	{
		return 9;
	}
	else if (strcmp(command,"quit") == 0)
	{
		return 10;
	}
	else
	{
		return -1;
	}
}

int hasPathName(char pathname[64])
{
	if (strcmp(pathname, "") == 0)
		return 0;
	return 1;
}

void breakupPathName()
{
	char temp[64], *tempItem, tempArray[64][64];
	int items = 1, i;

	// clear dirname and basename (just in case...)
	strcpy(dirname, "");
	strcpy(basename, "");

	// create copy of pathname to mess with
	strcpy(temp, pathname);

	// Create an array holding each token
	tempItem = strtok (temp, "/");
	strcpy (tempArray[0], tempItem);
	while (tempItem = strtok(0, "/"))
	{
		if (strcmp(tempItem, "") != 0)
		{
			strcpy(tempArray[items], tempItem);
			items++;
		}
	}

	// get the basename (last item in the array)
	strcpy(basename, tempArray[items - 1]);

	// get the dirname (everything *but* last item in the array)
	strcpy(dirname, "");
	for (i = 0; i < items - 1; i++)
	{
		strcat (dirname, "/");
		strcat (dirname, tempArray[i]);
	}
	if (strcmp(dirname, "") == 0)
	{
		strcpy(dirname, "/");
	}
}
