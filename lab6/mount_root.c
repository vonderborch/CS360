#include "type.h"

PROC *P[2] = {NULL};
MINODE *minode[100] = {NULL} ;
MINODE *root = NULL;
PROC *running = NULL;
PROC *readQueue = NULL;
int dev, offset = 1, ipos = INODE_START_POS, pathlen = 0, firstrun = 0, NINODES, NBLOCKS, startfromroot = 0;
char *device = "mydisk",
	pathname[64], command[64], *path[64];

int main(int argc, char *argv[])
{
	int i, cmdtype;
	char line[128], cname[64];
	if (argc > 1)
	{
		device = argv[1];
		printf("Device = %s\n", device);
	}
	init();
	mount_root();
	printf("> ");
	while(fgets(line, 128, stdin) != NULL)
	{
		if (strcmp(line, "") != 0 && strcmp(line, "\n") != 0)
		{
			line[strlen(line) - 1] = 0;
      			sscanf(line, "%s %s", command, pathname);
			cmdtype = find_cmd();
			switch (cmdtype)
			{
				case 0: menu(); break;
				case 1: ls(pathname); break;
				case 2: cd(pathname); break;
				case 3: mystat(pathname); break;
				case 41: quit(); break;
			}
		}
		memset(line, 0, sizeof(line));
		printf("> ");
	      	memset(command, 0, 64);
	      	memset(pathname, 0, 64);
	      	startfromroot = 0;
	}
	return 0;
}

int find_cmd()
{
	if (!strcmp(command, "menu"))
		return 0;
	else if (!strcmp(command, "ls"))
		return 1;
	else if (!strcmp(command, "cd"))
		return 2;
	else if (!strcmp(command, "stat"))
		return 3;
	else if (!strcmp(command, "quit") || !strcmp(command, "exit"))
		return 41;
	return -1;
}

MINODE *iget(int dev, int ino)
{
	MINODE *tmin = malloc(sizeof(MINODE));
	INODE *ti = malloc(sizeof(INODE));
	char buf[1024];
	ipos = (ino - 1)/8 + INODE_START_POS;
	offset = (ino -1) % 8;
	get_block(dev, ipos, buf);
	ti = (INODE*)buf + offset;
	if(S_ISDIR(ti->i_mode))
	{
		tmin->INODE = *ti;
		return tmin;
	}
	else
	{
		printf("A directory is not a file!\n");
		exit(0);
	}
}

void iput(MINODE *mip)
{
	int ino = getino(mip, ".");
	char buf[1024];
	mip->refCount--;
	if (mip->refCount)
		return;
	if (!mip->dirty)
		return;
	ipos = (ino - 1)/8 + INODE_START_POS;
	offset = (ino -1) % 8;
	get_block(dev, ipos, buf);
	INODE *ip = (INODE*)buf + offset;
	*ip = mip->INODE;
	put_block(dev, ipos, buf);
}

void init()
{
	int i;
	P[0] = malloc(sizeof(PROC));
	P[1] = malloc(sizeof(PROC));
	running = malloc(sizeof(PROC));
	readQueue = malloc(sizeof(PROC));
	root = malloc(sizeof(MINODE));
	P[0]->uid = 0;
	P[0]->pid = 1;
	P[1]->pid = 2;
	P[1]->uid = 1;
	P[0]->cwd = 0;
	P[1]->cwd = 0;
	running = P[0];
	readQueue = P[1];
	for(i = 0; i < 100; i++)
	{
		minode[i] = malloc(sizeof(MINODE));
		minode[i]->refCount=0;
	}
	root = 0;
}

void mount_root()
{
	char buf[1024];
  	dev = open(device, O_RDWR);
  	if (dev < 0)
	{
		printf("Cannot open %s\n", device);
		exit(0);
  	}
	root = iget(dev, 2);
	P[0]->cwd = iget(dev, 2);
	P[1]->cwd = iget(dev, 2);
	firstrun = 1;
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	NINODES = sp->s_inodes_count;
	NBLOCKS = sp->s_blocks_count;
	if(sp->s_magic !=  0xEF53 && sp->s_magic != 0xEF51)
	{
		printf("%s is not a valid device!\n", device);
		exit(0);
	}
}

int getino(MINODE *mp, char *name)
{
	char buf[1024], buf2[1024];
  	int i, j = 1, inodenumber, result = -7, size = 0;
  	get_block(dev, mp->INODE.i_block[0], buf2);
  	dp = (DIR *)buf2;
  	char *cp = buf2;
  	while(cp < buf2 + 1024)
	{
  		char temp[100];
  		strncpy(temp, dp->name, dp->name_len);
  		temp[dp->name_len] = 0;
  	 	if(strcmp(name, temp) == 0)
		{
  	 		inodenumber = dp->inode;
      			return inodenumber;
   		}
   		memset(temp, 0, 100);
    	cp += dp->rec_len;
    	dp = (DIR *)cp;
  }
  return result;
}

void menu()
{
	printf("Menu!\n");
}

void ls(char pathname[])
{
	int ino;
	MINODE *mip = malloc(sizeof(MINODE));
	mip = running->cwd;
	if(pathname[0])
	{
		int c;
		pathlen = tokenizer(pathname, &path, "/");
		if(startfromroot)
			mip = root;
		for(c = 0; c < pathlen; c++)
		{
			ino = getino(mip, path[c]);
			if(ino == -7)
			{
				printf("directory %s does not exist\n", path[c]);
				break;
			}
			mip = iget(dev,ino);
		}
	}
	printdir(mip->INODE);
}

void cd(char pathname[])
{
	int c, ino;
	MINODE *mip = malloc(sizeof(MINODE));
	mip = running->cwd;
	if(pathname[0])
	{
		pathlen = tokenizer(pathname, &path, "/");
		if(startfromroot)
			mip = root;	
		for(c = 0; c < pathlen; c++)
		{
			ino = getino(mip,path[c]);
			if(ino == -7)
			{
				printf("directory %s does not exist\n", path[c]);
				break;
			}
			mip = iget(dev, ino);
		}
		iput(running->cwd);
		running->cwd = mip;
	}
	else
	{
		iput(running->cwd);
		running->cwd = root;
	}
}

int ialloc(dev)
{
	// doesn't need to actually do anything else atm
	int bitpos;
	char buf[1024];
	get_block(dev, IBITMAP, buf);
	for (bitpos=0; bitpos < NINODES; bitpos++)
	{
		if (TST_bit(buf, bitpos)==0)
		{
		   SET_bit(buf, bitpos);
		   put_block(dev, IBITMAP, buf);
		   return bitpos+1;
		}
	}
	printf("Out of memory!\n");
	return 0;
}

int balloc(dev)
{
	// doesn't need to actually do anything else atm
	int bitpos;
	char buf[1024];
	get_block(dev, BBITMAP, buf);
	for (bitpos=0; bitpos < NBLOCKS; bitpos++)
	{
		if (TST_bit(buf, bitpos)==0){
		   SET_bit(buf, bitpos);
		   put_block(dev, BBITMAP, buf);
		   return bitpos+1;
		}
	}
	printf("Out of memory!\n");
	return 0;
}

int quit()
{
	// doesn't need to actually do anything else atm
	printf("Quiting!\n");
	exit(0);
}

void printdir(INODE ind){
	char buf[1024];
	get_block(dev,ind.i_block[0], buf);
	DIR *tp = (DIR*)buf;
	char *cp = buf;
	while(cp < buf + 1024){
	    char temp[100];
	    strncpy(temp, tp->name, tp->name_len);
	    temp[tp->name_len] = 0;
	    printf("%s   ",temp);
	    cp += tp->rec_len;
	    tp = (DIR *)cp;
	    memset(temp, 0, 100);
	}
	printf("\n");
}

int TST_bit(char buf[], int byte){
	int i, j;
	i = byte / 8;
	j = byte % 8;
	if ( buf[i] & (1 << j) )
		return 0;
	return 1;
}
void SET_bit(char buf[], int byte){
	int i, j;
	i = byte / 8;
	j = byte % 8;
	buf[i] |=  (1 << j);
}

void mystat(char pathname[]){
	int ino;
	if(strlen(pathname) > 0)
	{
		pathlen = tokenizer(pathname, &path, "/");
		MINODE *mip = malloc(sizeof(MINODE));
		mip = running->cwd;
		printf("path = %s", path[0]);
		ino = getino(mip,path[0]);
		if(ino == -7)
		{
			printf("directory %s does not exist\n", path[0]);
		}
		else
		{
			mip = iget(dev, ino);
			printf("Stats:\nst_mode = %d, st_nlink = %d, st_uid = %d, st_gid = %d, st_size = %d\n",mip->INODE.i_mode, mip->INODE.i_links_count, mip->INODE.i_uid, mip->INODE.i_gid, mip->INODE.i_size, mip->INODE.i_blocks);
			printf("st_blksize = 1024\nst_atime = %sst_mtime = %sst_ctime = %s\n", ctime(&(mip->INODE.i_atime)), ctime(&(mip->INODE.i_mtime)), ctime(&(mip->INODE.i_ctime)));
		}
	}
	else
	{
		printf("Error: Must have dirname!\n");
	}
}
