#include "functions.h"
#include "type.h"
#include "util.h"

// table of function pointers
functionTable ftable[] ={
	{"menu", CMD_MENU, "", ": display this help menu.", 2},
	{"help", CMD_MENU, "", ": display this help menu.", 2},
	{"quit", CMD_QUIT, "", ": exit the program.", 2},
	{"exit", CMD_QUIT, "", ": exit the program.", 2},
	{"clear", CMD_CLEAR, "", ": Clear the screen of text.", 2},
	{"reint", CMD_REINT, "", ": reinitialize the program. Not as reliable as manually restarting though.", 2},
	{"togpause", CMD_TOGGLESLEEP, "", ": Toggles pausing (for 3 second) after a command has been executed.", 2},
	{"prev", CMD_LAST, " [Command]", ": View or execute a previous command (defaults to display last commands, integer will execute the specified command).", 1},
	{"cd", CMD_CD, " [path]", ": Change cwd to the path (defaults to root directory).", 1},
	{"ls", CMD_LS, " [path]", ": List all files in the path (defaults to cwd).", 1},
	{"mkdir", CMD_MKDIR, " <name>", ": make a directory with name.", 0},
	{"creat", CMD_CREAT, " <name>", ": make a file with name.", 0},
	{"rmdir", CMD_RMDIR, " <name>", ": remove a directory with name.", 0},
	{"rm", CMD_UNLINK, " <name>", ": remove a file with name.", 0},
	{"link", CMD_LINK, " <name> <newname>:", " Hard link file with name to file with newname.", 0},
	{"unlink", CMD_UNLINK, " <name>", ": Unlink file with name.", 0},
	{"symlink", CMD_SYMLINK, " <name> <newname>", ": Symlink file with name to file with newname.", 0},
	{"touch", CMD_TOUCH, " <name>", ": Touch a file with name.", 0},
	{"chmod", CMD_CHMOD, " <name> <perms>", ": Change the permissions (0777-style) of the file with name.", 0},
	{"chown", CMD_CHOWN, " <name> <uid>", ": Change the owner of the file with name.", 0},
	{"stat", CMD_STAT, " <name>", ": Display information on file with name.", 0},
	{"chgrp", CMD_CHGRP, " <name> <newgroup>", ": Change the group of the file to newgroup.", 0},
	{"pwd", CMD_PWD, "", ": Print the current working directory (cwd).", 2},
	{0, 0, 0, 0, 0}
};

void main(int argc, char* argv[])
{
	char line[128], cname[64];
	int i, searching = 1;

	if (argc > 1)
		init(argv[1]);
	else
		init("");
	
	// tell user about help menu on startup
	printf("*********************************************\n");
	startuphelp();

	while(1)
	{
		printf("Running Process: P%d\n",running->pid);
		printf("> ");
		fgets(line, 128, stdin);
		line[strlen(line)-1] = 0;
		if(!line[0])
			continue;

		// reset input variables
		memset(pathname, 0, 256);
		memset(parameter, 0, 256);
		
		// get the command, pathname, and parameter variables
		sscanf(line, "%s %s %64c", cname, pathname, parameter);
		
		// find and execute the command
		find_and_execute_command(cname);
		
		// save command
		savecommand(line);
		
		if (sleepmode)
			sleep(2);
	}
}
