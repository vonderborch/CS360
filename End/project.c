#include "functions.c"

int main(int argc, char* argv[])
{
	int (*p[27]) (char pathname[], char *parameter);
	int i = 0, cmd;
	char line[128];
	if (argc > 1)
	{
		device = argv[1];
		printf("Device = %s\n", device);
	}
	init();
	
	// initialize function table
	p[0] = menu;
	p[1] = make_dir;
	p[2] = cd;
	p[3] = pwd;
	p[4] = ls;
	p[5] = remove_dir;
	p[6] = creat_file;
	p[7] = linker;
	p[8] = ulink;
	p[9] = sylink;
	p[10] = rm_file;
	p[11] = chmod_file;
	p[12] = chown_file;
	p[13] = stater;
	p[14] = touch_file;
	p[15] = open_file;
	p[16] = close_file;
	p[17] = read_file;
	p[18] = write_file;
	p[19] = lseek_file;
	p[20] = cat_file;
	p[21] = cp_file;
	p[22] = mv_file;
	p[23] = mount;
	p[24] = umount;
	p[25] = quit;
	p[26] = wrongcmd;
	
	while(1)
	{
		char *name_of_dir = NULL;
		i = 0;
		while(minode[i]->refCount != 0)
		{
		  printf("minode[%d]->refCount: %d\n", i, minode[i]->refCount);
		  i++;
		}
		printf("Running: P%d\n", running->pid);
		printf("> ");
		fgets(line, 128, stdin);
		line[strlen(line)-1] = 0;
      	sscanf(line, "%s %s %s", command, pathname, parameter);
      	cmd = findCmd();
		(*p[cmd])(pathname, parameter);
        memset(path, 0, 64);
      	memset(command, 0, 64);
      	memset(pathname, 0, 64);
      	memset(parameter, 0, 64);
      	startfromroot = 0;
        pathlen = 0;
	}
	return 0;
}

int findCmd()
{
  if(!strcmp("menu",command))
    return 0;
  else if(!strcmp("mkdir", command))
    return 1;
  else if(!strcmp("cd", command))
    return 2;
  else if(!strcmp("pwd", command))
    return 3;
  else if(!strcmp("ls", command))
    return 4;
  else if(!strcmp("rmdir", command))
    return 5;
  else if(!strcmp("creat", command))
    return 6;
  else if(!strcmp("link", command))
    return 7;
  else if(!strcmp("unlink", command))
    return 8;
  else if(!strcmp("symlink", command))
    return 9;
  else if(!strcmp("rm", command))
    return 10;
  else if(!strcmp("chmod", command))
    return 11;
  else if(!strcmp("chown", command))
    return 12;
  else if(!strcmp("stat", command))
    return 13;
  else if(!strcmp("touch", command))
    return 14;
  else if(!strcmp("open", command))
    return 15;
  else if(!strcmp("close", command))
    return 16;
  else if(!strcmp("read", command))
    return 17;
  else if(!strcmp("write", command))
    return 18;
  else if(!strcmp("lseek", command))
    return 19;
  else if(!strcmp("cat", command))
    return 20;
  else if(!strcmp("cp", command))
    return 21;
  else if(!strcmp("mv", command))
    return 22;
  else if(!strcmp("mount", command))
    return 23;
  else if(!strcmp("umount", command))
    return 24;
  else if(!strcmp("quit",command) || !(strcmp("exit", command)))
    return 25;
  return 26;
}