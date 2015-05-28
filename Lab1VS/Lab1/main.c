/**********************************************************************************************
 * Programmer: Christian Webber
 * Group #1, Seq #19
 * Class: CptS 360, Spring 2014;
 * Lab 1
 * Created: January 29th, 2014
 * Last Revised: January 30th, 2014
 * File: main.c
 *********************************************************************************************/

#include "common.h"

// global variables
NODE *root, *cwd, *dirNode;
char line[128];
char command[64], pathname[64];
char dirname[64], basename[64];
char cwdString[64];

int main(void)
{
	// Setup basic variables
	int isRunning = 1, cid = 0, good;

	// initialize the program
	good = initialize();
	if (good == 1)
	{
		printf("Root initialized!\n");
		printf("Enter 'help' (without quotes) to return list of commands.\n");
	}
	else
	{
		printf("Root not initialized, not enough space!\n");
		isRunning = 0;
	}

	// main program loop
	while (isRunning == 1)
	{
		// reset line, dirname, basename, command, and pathname
		memset(line, 0, sizeof(line));
		memset(dirname, 0, sizeof(dirname));
		memset(basename, 0, sizeof(basename));
		memset(command, 0, sizeof(command));
		memset(pathname, 0, sizeof(pathname));

		// get input
		printf("\nCommand? ");
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
		case 11:
			// reint
			reint();
			break;
		}

	}

	return 0;
}

// loading!
int initialize()
{
	NODE *nNew = NULL;
	nNew = (NODE *) malloc (sizeof (NODE));
	if (nNew != NULL)
	{
		// create a new node to act as the root directory and assign *root and *cwd to it
		strcpy(nNew->name, "");
		nNew->type = 'D';
		nNew->childPtr = NULL;
		nNew->siblingPtr = NULL;
		nNew->parentPtr = NULL;
		root = nNew;
		cwd = root;
		return 1;
	}
	return 0;
}

// what to do when the user asks to reint
void reint()
{
	printf("******** reint ********\n");

	clearer(&root); // clear the current file tree
	initialize(); // initialize a new file tree

	printf("File tree reinitialized.\n");
}

// what to do when an invalid command is present
void invalid()
{
	printf("Invalid command, please enter a valid command or enter 'help' for help!\n");
}

// what to do when the user asks for help
void help()
{
	printf("Possible Commands:\n   help - displays help\n   mkdir <pathname> - makes a new directory in under pathname\n   rmdir <pathname> - removes the specified directory\n   cd [pathname] - moves cwd to the specified directory. If no pathname is given, returns cwd to route.\n   ls [pathname] - displays all contents of the directory listed (or cwd if no directory is listed)\n   pwd - print the absolute path of the cwd\n   creat <pathname> - creates a file at the specified directory\n   rm <pathname> removes a file at the specified directory\n   save <filename> - save the current file system tree to a file\n   reload <filename> - load a file system tree from a file\n   reint - clears the current file tree\n   quit - quits the program\n");
}

// what to do when the user asks to mkdir
void mkdir()
{
	printf("******** mkdir ********\n");
	if (hasPathName(pathname) == 0) // has the user specified a pathname?(required)
		printf("Invalid command format, please specify a pathname!\n");
	else
		addNode('D'); // add a new directory node
}

// what to do when the user asks to rmdir
void rmdir()
{
	printf("******** rmdir ********\n");
	if (hasPathName(pathname) == 0) // has the user specified a pathname?(required)
		printf("Invalid command format, please specify a pathname!\n");
	else
		removeNode('D'); // remove a directory node
}

// what to do when the user asks to creat
void creat()
{
	printf("******** creat ********\n");
	if (hasPathName(pathname) == 0) // has the user specified a pathname?(required)
		printf("Invalid command format, please specify a pathname!\n");
	else
		addNode('F'); // add a new file node
}

// what to do when the user asks to rm
void rm()
{
	printf("******** rm ********\n");
	if (hasPathName(pathname) == 0) // has the user specified a pathname?(required)
		printf("Invalid command format, please specify a pathname!\n");
	else
		removeNode('F'); // remove a file node
}

// what to do when the user asks to cd
void cd()
{
	printf("******** cd ********\n");

	if (hasPathName(pathname) == 0) // has the user specified a pathname?(optional)
	{
		cwd = root; // set cwd to root
		printf("Current working directory set to root!\n");
	}
	else
	{
		int good = 0;

		// update dirnode to the current pathname
		good = getDirNode();

		if (good == 1)
		{
			// now we need to find basename...
			NODE *tempNode = NULL, *tempLastNode = NULL;
			tempNode = (NODE *) malloc (sizeof (NODE));
			tempLastNode = (NODE *) malloc (sizeof (NODE));
			if (tempNode != NULL && tempLastNode != NULL)
			{
				tempNode = dirNode; // assign tempNode the value of dirNode

				// search for basename if there's a child node for the current directory (otherwise we don't need to worry about siblings, etc.)
				if (tempNode->childPtr != NULL)
				{
					tempLastNode = NULL;
					tempNode = tempNode->childPtr;
					while (tempNode != NULL)
					{
						if (strcmp(tempNode->name, basename) == 0) // we found the node we're looking for!
							break;
						else if (tempNode->siblingPtr == NULL) // we found the last node, so the directory doesn't exist :(
						{
							printf("Directory does not exist!\n");
							return;
						}
						else // continue searching!
						{
							tempLastNode = tempNode;
							tempNode = tempNode->siblingPtr;
						}
					}

					// assuming we've made it here, we've found the node we're looking for...
					// if its a directory, we can point the cwd to it!
					if (tempNode->type == 'D')
					{
						cwd = tempNode;
						printf("Directory changed!\n");
						return;
					}
					else
					{
						printf("Cannot change directory, directory not of type directory!\n");
						return;
					}
				}
			}
		}
		return;
	}
}

// what to do when the user asks to ls
void ls()
{
	int good = 0;
	printf("******** ls ********\n");

	if (hasPathName(pathname) == 0)
	{
		NODE *tempNode = NULL;
		tempNode = (NODE *) malloc (sizeof (NODE));
		if (tempNode != NULL)
		{
			tempNode = cwd; // set tempnode to the cwd
			printf("Contents of directory:\nTYPE\tNAME\n"); // print "table" keys
			tempNode = tempNode->childPtr;
			while (tempNode != NULL) // list the contents of this directory
			{
				printf("%c\t%s\n", tempNode->type, tempNode->name);
				tempNode = tempNode->siblingPtr;
			}
		}
	}
	else
	{
		// update dirnode to the current pathname
		good = getDirNode();

		if (good == 1)
		{
			// now we need to find basename...
			NODE *tempNode = NULL;
			tempNode = (NODE *) malloc (sizeof (NODE));
			if (tempNode != NULL)
			{
				tempNode = dirNode; // assign tempNode the value of dirNode

				// search for basename if there's a child node for the current directory (otherwise we don't need to worry about siblings, etc.)
				if (tempNode->childPtr != NULL)
				{
					tempNode = tempNode->childPtr;
					while (tempNode != NULL)
					{
						if (strcmp(tempNode->name, basename) == 0) // we found the node we're looking for!
							break;
						else if (tempNode->siblingPtr == NULL) // we found the last node, so the directory doesn't exist :(
						{
							printf("Directory does not exist!\n");
							return;
						}
						else // continue searching!
						{
							tempNode = tempNode->siblingPtr;
						}
					}

					// assuming we've made it here, we've found the node we're looking for...
					// if its a directory, we can print out all its children
					if (tempNode->type == 'D')
					{
						printf("Contents of directory:\nTYPE\tNAME"); // print "table" keys
						tempNode = tempNode->childPtr;
						while (tempNode != NULL) // list the contents of this directory
						{
							printf("%c\t%s\n", tempNode->type, tempNode->name);
							tempNode = tempNode->siblingPtr;
						}
						return;
					}
					else
					{
						printf("Cannot access children, path lead to a file not a directory!\n");
						return;
					}
				}
			}
		}
	}
}

// what to do when the user asks to pwd
void pwd()
{
	NODE *tempNode = NULL;
	printf("******** pwd ********\n");
	
	// get string (reversed) of current working directory
	tempNode = (NODE *) malloc (sizeof (NODE));
	if (tempNode != NULL)
	{
		updateCWDString(); // get the cwd string (update it)
		printf("Current Working Directory: %s\n", cwdString);
	}
	else
	{
		printf("Faild to list directory nodes, not enough space for operations!\n");
	}
}

// what to do when the user asks to save
void save()
{
	printf("******** save ********\n");
	if (hasPathName(pathname) == 0) // has the user specified a filename?(required)
	{
		printf("Invalid format, please specify a filename!\n");
		return;
	}
	else
	{
		NODE *cwdBackup = NULL;
	
		cwdBackup = (NODE *) malloc (sizeof (NODE));
		if (cwdBackup != NULL)
		{
			// open the file
			FILE *output = fopen(pathname, "w+");

			// backup cwd
			cwdBackup = cwd;

			// save the file
			printf("saving");
			saver(output, root);

			// ??????

			// profit
			fclose(output);
			cwd = cwdBackup;
			printf("\nFile tree saved!\n");
		}
	}
}

// what to do when the user asks to reload
void reload()
{
	printf("******** reload ********\n");
	if (hasPathName(pathname) == 0) // has the user specified a filename?(required)
	{
		printf("Invalid format, please specify a filename!\n");
		return;
	}
	else
	{
		// open the file
		FILE *input = fopen(pathname, "r");

		// clear and reinitialize the current tree
		clearer(&root);
		initialize();

		// load the file
		while (!feof(input))
		{
			char type;
			fscanf(input, "%c %s\n", &type, &pathname);

			if (strcmp(pathname, "/") != 0) // ignore the root directory (already created)
			{
				if (type == 'D')
				{
					mkdir(); // if the next node should be a directory, make a new directory
				}
				else if (type == 'F')
				{
					creat(); // if the next node should be a file, make a new file
				}
				else //otherwise, the save file is invalid
				{
					printf("A node type is an invalid type, cannot load file!\n");
					clearer(&root);
					initialize();
					return;
				}
			}
		}

		// cleanup
		fclose(input);
		cwd = root;
		printf("\nFile tree loaded!\n");
	}
}

// recursively save a file
void saver(FILE *output, NODE *curnode)
{
	NODE *tempChild = NULL;
	
	tempChild = (NODE *) malloc (sizeof (NODE));
	if (tempChild != NULL)
	{
		cwd = curnode;
		// update path
		updateCWDString();

		// save self
		printf(".");
		fprintf(output, "%c %s\n", curnode->type, cwdString);

		// loop through children and save them...
		tempChild = curnode->childPtr;
		while (tempChild != NULL)
		{
			saver(output, tempChild);
			tempChild = tempChild->siblingPtr;
		}
	}
	else
		printf("Failed to save part of tree, not enough space!\n");
}

// recursively clear the current tree
void clearer(NODE **curnode)
{
	if (*curnode)
	{
		// clear children
		clearer(&(*curnode)->childPtr);
		// clear siblings
		clearer(&(*curnode)->siblingPtr);
		//clear self
		free(*curnode);
		*curnode = NULL;
	}
}

void removeNode(char type)
{
	int good = 0;

	// update dirnode to the current pathname
	good = getDirNode();

	// if we got the directory correctly...
	if (good == 1)
	{
		NODE *tempNode = NULL, *tempLastNode = NULL;
		tempNode = (NODE *) malloc (sizeof (NODE));
		tempLastNode = (NODE *) malloc (sizeof (NODE));
		if (tempNode != NULL && tempLastNode != NULL)
		{
			tempNode = dirNode; // assign tempNode the value of dirNode

			// search for basename if there's a child node for the current directory (otherwise we don't need to worry about siblings, etc.)
			if (tempNode->childPtr != NULL)
			{
				tempLastNode = NULL;
				tempNode = tempNode->childPtr;
				while (tempNode != NULL)
				{
					if (strcmp(tempNode->name, basename) == 0) // we found the node we're looking for!
						break;
					else if (tempNode->siblingPtr == NULL) // we found the last node, so the directory doesn't exist :(
					{
						printf("Path does not exist!\n");
						return;
					}
					else // continue searching!
					{
						tempLastNode = tempNode;
						tempNode = tempNode->siblingPtr;
					}
				}

				// assuming we've made it here, we've found the node we're looking for...
				// if it's empty and a directory, we can delete it, otherwise we can't
				if (tempNode->childPtr == NULL)
				{
					if (type == 'D')
					{
						if (tempNode->type == 'D')
						{
							if (tempLastNode == NULL) // if there arn't sibling nodes...
							{
								tempNode->parentPtr->childPtr = NULL;
							}
							else // if there are sibling nodes...
							{
								tempLastNode->siblingPtr = tempNode->siblingPtr;
							}
							free(tempNode);
							printf("Directory deleted!\n");
							return;
						}
						else
						{
							printf("Cannot delete, path lead to a file!\n");
							return;
						}
					}
					else if (type == 'F')
					{
						if (tempNode->type == 'F')
						{
							if (tempLastNode == NULL) // if there arn't sibling nodes...
							{
								tempNode->parentPtr->childPtr = NULL;
							}
							else // if there are sibling nodes...
							{
								tempLastNode->siblingPtr = tempNode->siblingPtr;
							}
							free(tempNode);
							printf("File deleted!\n");
							return;
						}
						else
						{
							printf("Cannot delete, path lead to a directory!\n");
							return;
						}
					}
				}
				else
				{
					printf("Cannot delete, directory not empty!\n");
					return;
				}
			}
		}
	}
}

void addNode(char type)
{
	int good = 0;

	// update dirnode to the current pathname
	good = getDirNode();

	// if we got the directory correctly...
	if (good == 1)
	{
		NODE *tempNode = NULL, *nNode = NULL;
		tempNode = (NODE *) malloc (sizeof (NODE));
		if (tempNode != NULL)
		{
			tempNode = dirNode; // assign tempNode the value of dirNode

			// search for basename if there's a child node for the current directory (otherwise we don't need to worry about siblings, etc.)
			if (tempNode->childPtr != NULL)
			{
				tempNode = tempNode->childPtr;
				while (tempNode != NULL)
				{
					if (strcmp(tempNode->name, basename) == 0)
					{
						printf("Name already exists!\n");
						return;
					}
					else if (tempNode->siblingPtr == NULL) // we found the last node, so we're done searching!
						break;
					else // continue searching!
						tempNode = tempNode->siblingPtr;
				}
			}

			// since it doesn't already exist, create a new dir...
			// dirNode = parent node
			// tempNode = sibling node
			nNode = (NODE *) malloc (sizeof (NODE));
			if (nNode != NULL)
			{
				strcpy(nNode->name, basename);
				nNode->type = type;
				nNode->siblingPtr = NULL;
				nNode->childPtr = NULL;
				nNode->parentPtr = dirNode;

				if (dirNode->childPtr == NULL)
					dirNode->childPtr = nNode;
				else
					tempNode->siblingPtr = nNode;
				if (type == 'D')
					printf("Directory added!\n");
				else if (type == 'F')
					printf("File added!\n");
				return;
			}
		}
	}
}

// find the command id
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
	else if (strcmp(command,"reint") == 0)
	{
		return 11;
	}
	else
	{
		return -1;
	}
}

// is a pathname specified?
int hasPathName(char pathname[64])
{
	if (strcmp(pathname, "") == 0)
		return 0;
	return 1;
}

// is the path absolute or relative?
int isAbsolute(char path[64])
{
	if (path[0] != '/')
		return 1;
	return 0;
}

void updateCWDString()
{
	NODE *tempNode = NULL;
	// get string (reversed) of current working directory
	tempNode = (NODE *) malloc (sizeof (NODE));
	if (tempNode != NULL)
	{
		int length=0,i=0,j=0;
		char tempArray[64][64];

		// set tempnode to the current working directory (cwd)
		tempNode = cwd;

		// go through nodes and generate a reversed array of the cwd
		while (tempNode->parentPtr != NULL)
		{
			strcpy(tempArray[length],tempNode->name);
			length++;
			tempNode = tempNode->parentPtr;
		}
		
		// clear cwdstring
		memset(cwdString, 0, sizeof(cwdString));

		// go through array (starting at the end) and add items to the cwdstring
		if (length > 0)
		{
			for (i = length; i > 0; i--)
			{
				strcat(cwdString, "/");
				strcat(cwdString, tempArray[i - 1]);
			}
		}
		else
			strcpy(cwdString, "/");
	}
	else
	{
		printf("Couldn't compenstate for the absolute working directory!\n");
		return;
	}
}

// update dirNode
int getDirNode()
{
	NODE *tempNode = NULL;
	int good = 0;

	// breakup pathname
	breakupPathName();
	printf("Dirname: %s, Basename: %s\n", dirname, basename);

	// search for dirnode (assuming the root isn't the node called for...
	tempNode = (NODE *) malloc (sizeof (NODE));
	dirNode = (NODE *) malloc (sizeof (NODE));
	if (tempNode != NULL)
	{
		tempNode = root;
		// do we need to search? if not, then we don't need to search for the correct dir!
		if (strcmp(dirname, "/") != 0)
		{
			char *tempDir;

			// if we do...
			tempDir = strtok(dirname, "/"); // tokenize dirname
			tempNode = tempNode->childPtr; // point at the child pointer
			// search through siblings for the correct dir...
			while (tempNode != NULL)
			{
				if (strcmp(tempNode->name, tempDir) == 0)
					break;
				else
					tempNode = tempNode->siblingPtr;
			}
			if (tempNode == NULL)
			{
				printf("Directory not found!\n");
				return 0;
			}
			if (tempNode->type == 'F')
			{
				printf("Path leads to a file, not a directory!\n");
				return 0;
			}

			// now that the root's directory has been looked at, go through the children of the directory we're looking at...
			while (tempDir = strtok(0, "/"))
			{
				good = 0;
				tempNode = tempNode->childPtr;
				// search through siblings for the correct dir...
				while (tempNode != NULL)
				{
					if (strcmp(tempNode->name, tempDir) == 0) // correct child has been found, we can look for the next token in its children if we need to
					{
						good = 1;
						break;
					}
					else
						tempNode = tempNode->siblingPtr;
				}
				if (tempNode == NULL)
				{
					printf("Directory not found!\n");
					return 0;
				}
				if (tempNode->type == 'F')
				{
					printf("Path leads to a file, not a directory!\n");
					return 0;
				}
			}
		}

		// if dirnode exists...
		dirNode = tempNode;
		return 1;
	}

	printf("Directory failed to be added, not enough space!\n");
	return 0;
}

// breakup the pathname into dirname and basename parts
void breakupPathName()
{
	char temp[64], *tempItem, tempArray[64][64];
	int items = 1, i, absolute = 0, hasCompensated = 0;

	// determine if pathname is absolute or relative
	absolute = isAbsolute(pathname);

	// clear dirname and basename (just in case...)
	strcpy(dirname, "");
	strcpy(basename, "");

	// create copy of pathname to mess with
	strcpy(temp, pathname);

	// Create an array holding each token
	if (strcmp (temp, "/") != 0 && strcmp(temp, "") != 0)
	{
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
			if (hasCompensated == 0 && absolute == 1)
			{
				NODE *tempNode = NULL;
				// get string (reversed) of current working directory
				tempNode = (NODE *) malloc (sizeof (NODE));
				if (tempNode != NULL)
				{
					updateCWDString();
					strcpy(dirname, cwdString);
					hasCompensated = 1;
				}
				else
				{
					printf("Couldn't compenstate for the absolute working directory!\n");
					return;
				}
			}
			else
			{
				strcat (dirname, "/");
			}
			strcat (dirname, tempArray[i]);
		}
		if (strcmp(dirname, "") == 0)
		{
			strcpy(dirname, "/");
		}
	}
	else
	{
		strcpy(dirname, "/");
	}
}

