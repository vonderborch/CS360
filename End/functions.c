#include "util.c"

//stat
void stater(char pathname[])
{
	int ino;
	if(pathname[0])
	{
		MINODE *mip = malloc(sizeof(MINODE));
		tokenizer(pathname);
		mip = running->cwd;
		ino = getino(mip,path[0]);
		if(ino == -7)
		{
			printf("directory %s does not exist\n", path[0]);
		}
		else
		{
			mip = iget(dev, ino);
			printf("File: '%s'\n", path[0]);
			printf("Size: %d\t\tBlocks: %d\t\tIO BLOCK: \n", mip->INODE.i_size,mip->INODE.i_blocks);
			printf("Stats:\nst_mode = %d, st_nlink = %d, st_uid = %d, st_gid = %d, st_size = %d\n",mip->INODE.i_mode, mip->INODE.i_links_count, mip->INODE.i_uid, mip->INODE.i_gid, mip->INODE.i_blocks);
			printf("st_blksize = 1024\nAccess: %sModify: %sChange: %s\n", ctime(&(mip->INODE.i_atime)), ctime(&(mip->INODE.i_mtime)), ctime(&(mip->INODE.i_ctime)));
		}
		iput(mip);
	}
	else
	{
		printf("Kinda need a dirname to stat with\n");
	}

}

//ls
void ls(char pathname[], char *parameter)
{
	int ino;
	MINODE *mip = NULL;
	mip = running->cwd;
	printf("running->cwd.ino = %d\n", running->cwd->ino);
	if(pathname[0])
	{
		printf("path\n");
		int c;
		tokenizer(pathname);
		if(startfromroot == 1){mip = root;}
		for(c = 0; c < pathlen; c++)
		{
			ino = getino(mip, path[c]);
			if(ino == -7)
			{
				printf("directory %s does not exist\n", path[c]);
				iput(mip);
				return;
			}
			mip = iget(dev,ino);
			if(!S_ISDIR(mip->INODE.i_mode))
			{
				iput(mip);
				return;}
			iput(mip);
		}
	}
	printf("running->cwd->INODE.i_block[0] = %d\n", running->cwd->INODE.i_block[0]);
	printf("no path\n");
	printdir(mip->INODE);
}

//cd
void cd(char pathname[], char *parameter)
{
	int c, ino;
	MINODE *mip = malloc(sizeof(MINODE));
	mip = running->cwd;
	printf("mip\n");
	if(pathname[0])
	{
		tokenizer(pathname);
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
			if(!S_ISDIR(mip->INODE.i_mode))
			{
				printf("%s is not a directory\n", path[c]);
				return;
			}
		}
		iput(running->cwd);
		for(c = 0; c < NMINODES; c++)
		{
			if(mip == minode[c]){
				minode[c]->refCount++;
				running->cwd = minode[c];
				break;
			}
			else if(!minode[c]){
				minode[c] = mip;
				minode[c]->refCount++;
				running->cwd = minode[c];
				break;
			}
		}
	}
	else
	{
		iput(running->cwd);
		running->cwd = root;
		running->cwd->refCount++;
	}
}

//pwd
void pwd()
{
	int counter = 0, ino = 12, parentino = 0, n;
	char *name = NULL, *patharray[100];
	memset(path, 0, 100);
	MINODE *mip = malloc(sizeof(MINODE));
	MINODE *parentmip = malloc(sizeof(MINODE));
	mip = running->cwd;
	if(mip == root)
	{
		printf("/\n");
		return;
	}
	while(ino != parentino)
	{
		printf("check\n");
		findino(mip, &ino, &parentino);
		parentmip = iget(dev, parentino);
		findmyname(parentmip, ino, &name);
		patharray[counter] = malloc(sizeof(name));
		patharray[counter] = name;
		counter++;
		iput(parentmip);
		mip = parentmip;
	}
	for(n = counter-2; n >= 0; n--)
		printf("/%s",patharray[n]);
	printf("\n");
}

//cat
void cat_file(char pathname[])
{
	int fd, c, ino, i, n;
	char line[128], buf[1024];
	MINODE *mip = malloc(sizeof(MINODE));
	mip = running->cwd;
	if(pathname[0])
	{
		tokenizer(pathname);
		if(startfromroot)
			mip = root;
		for(c = 0; c < pathlen; c++)
		{
			if(c == pathlen -1)
			{
				printf("c = %d, pathlen = %d and path[c] = %s", c, pathlen, path[c]);
				//c = pathlen +1;
				fd = open(path[c], O_RDONLY);
				if(fd < 0)
				{
					printf("open %s failed\n", path[c]);
					return;
				}
				while(n = read(fd, buf, 1024))
				{
					for(i = 0; i <n; i++)
					{
						if(buf[i] == '/' && buf[i+1] == 'n')
						{
							putchar('/');
							putchar('/r');
							i += 2;
						}
						else
						{
							putchar(buf[i]);
						}
					}
				}
				
			}
			else
			{
				ino = getino(mip,path[c]);
				if(ino == -7)
				{
					printf("directory %s does not exist\n", path[c]);
					break;
				}
				mip = iget(dev, ino);
				if(!S_ISDIR(mip->INODE.i_mode))
					return;
			}
		}
		iput(running->cwd);
		running->cwd = mip;
	}
	else
	{
		while(1)
		{
			printf("\n");
			fgets(line, 128, stdin);
			line[strlen(line)-1] = 0;
			printf("%s",line);
			memset(line, 128, 0);
		}
	}
}	

// mkdir
int make_dir(char pathname[], char *parameter)
{
	int r;
	MINODE *pip = NULL;
	char *child = NULL;
	complete_tokenizer(pathname, &child, &pip);
	printf("child = %s\n",child);
	printf("pip ino = %d and running->cwd->ino = %d\n", pip->ino, running->cwd->ino);
	r = getino(pip, child);
	if(r > 0)
	{
		printf("There is already a directory/file named %s in this directory\n", child);
		return -1;
	}
	r = my_mkdir(pip, child);
	pip->dirty = 1;
  	pip->INODE.i_links_count++;
  	iput(pip);
}

// rmdir
int remove_dir(char pathname[], char *parameter)
{
	int r, c, ino, parentino, i;
	MINODE *pip = malloc(sizeof(MINODE));
	MINODE *mip = malloc(sizeof(MINODE));
	char *child = NULL;
	if(pathname[0])
	{
		complete_tokenizer(pathname, &child, &pip);
		if(startfromroot)
			mip = root;
		else
			mip = running->cwd;	
		for(c = 0; c < pathlen; c++)
		{
			ino = getino(mip,path[c]);
			if(ino == -7)
			{
				printf("directory %s does not exist\n", path[c]);
				return -1;
			}
			mip = iget(mip->dev, ino);
			if(!S_ISDIR(mip->INODE.i_mode))
			{
				printf("mip->INODE.i_mode = %x\n", mip->INODE.i_mode);
				printf("%s is not a directory\n", path[c]);
				return -1;
			}
		}
		iput(running->cwd);
	}
	else
	{
		printf("Invalid use of rmdir\n");
		return -1;
	}
	r = checkdir(mip);
	if(r < 0)
		return -1;
	for (i=0; i<12; i++)
	{
    	if(mip->INODE.i_block[i]==0)
    		bdalloc(mip->dev, mip->INODE.i_block[i]);
  	}
  	idalloc(mip->dev, mip->ino);
  	iput(mip);
	r = my_rmdir(pip, child);
	pip->INODE.i_links_count --;
	pip->dirty = 1;
	iput(pip);
}

//creat
int creat_file(char pathname[], char *parameter)
{
	int r;
	MINODE *pip = malloc(sizeof(MINODE));
	char *child = NULL;
	complete_tokenizer(pathname, &child, &pip);
	printf("child = %s\n",child);
	printf("pip ino = %d and running->cwd->ino = %d\n", pip->ino, running->cwd->ino);
	if(getino(pip, child) > 0)
	{
		printf("There is already a directory/file named %s in this directory\n", child);
		return -1;
	}
	r = my_mkfile(pip, child);
	pip->dirty =1;
	iput(pip);
}

// rm
int rm_file(char pathname[], char *parameter)
{
	int r = 0, c, ino, parentino, i;
	MINODE *pip = NULL;
	MINODE *mip = NULL;
	char *child = NULL;
	if(pathname[0])
	{
		complete_tokenizer(pathname, &child, &pip);
		if(startfromroot)
			mip = root;
		else
			mip = running->cwd;
		for(c = 0; c < pathlen; c++)
		{
			ino = getino(mip,path[c]);
			if(ino == -7)
			{
				printf("directory %s does not exist\n", path[c]);
				return -1;
			}
			mip = iget(mip->dev, ino);
			if(c != pathlen -1 &&!S_ISDIR(mip->INODE.i_mode))
			{
				printf("%s is not a directory\n", path[c]);
				return -1;
			}
		}
		iput(running->cwd);
	}
	else
	{
		printf("Invalid use of rm\n");
		return -1;
	}
	if(!S_ISREG(mip->INODE.i_mode))
	{
		printf("mip->INODE.i_mode = %x\n", mip->INODE.i_mode);
		printf("%s is not a file\n", child);
		printf("normally it would exit here but seeing as my modes are off oh well\n");
		getchar();
	}
	for (i=0; i<12; i++)
	{
    	if(mip->INODE.i_block[i]==0)
    		bdalloc(mip->dev, mip->INODE.i_block[i]);
  	}
  	idalloc(mip->dev, mip->ino);
  	iput(mip);
	r = my_rmdir(pip, child);
	pip->dirty = 1;
	iput(pip);
}

// link
void linker(char pathname[], char *parameter)
{
	int i = -1, ino;
	char *name = NULL;
	char *newname = NULL;
	MINODE *first = NULL;
	MINODE *second = NULL;
	complete_tokenizer(parameter, &newname, &second);
	complete_tokenizer(pathname, &name, &first);
	printf("first->ino = %d and second->ino = %d\n", first->ino, second->ino);
	if(getino(second, newname) > 0)
	{
		printf("There is already a directory/file named %s in this directory\n", newname);
		return -1;
	}
	linkit(first, second, name, newname);
	second->INODE.i_links_count++;
	second->dirty = 1;
	iput(second);
}

// unlink
void ulink(char pathname[], char *parameter)
{
	int r;
	char *name = NULL;
	MINODE *minode = NULL;
	complete_tokenizer(pathname, &name, &minode);
	if(!S_ISREG(minode->INODE.i_mode))
	{
		printf("NOT A FILE\n");
		return -1;}
	minode->INODE.i_links_count--;
	if((minode->INODE.i_links_count) <= 0)
	{
		idalloc(minode->dev, minode->ino);
		bdalloc(minode->dev, minode->ino);
	}
	r = my_rmdir(minode, name);
}

// symlink
void sylink(char pathname[], char *parameter)
{
	printf("symlink\n");
	/*3. 	symlink oldNAME  newNAME    e.g. symlink /a/b/c /x/y/z
			ASSUME: oldNAME has <= 60 chars, inlcuding the NULL byte.
			(INODE has 24 UNUSED bytes after i_block[]. So may use up to 84 bytes for oldNAME) 

			(1). verify oldNAME exists (either a DIR or a FILE)
			(2). creat a FILE /x/y/z
			(3). change /x/y/z's type to S_IFLNK (0120000)=(1010.....)=0xA...
			(4). write the string oldNAME into the i_block[ ], which has room for 60 chars.
			(5). write the INODE of /x/y/z back to disk.
	*/
}

// chmod
void chmod_file(char pathname[], char *parameter)
{
	int i = -1, j, ino, r, w, x;
	char *name = NULL, temp;
	MINODE *edfile = NULL;
	complete_tokenizer(pathname, &name, &edfile);
	
	if (strlen(parameter) == 4)
	{
		printf("Old Permissions: %x\n", edfile->INODE.i_mode);
		// reset file perms
		for (j = 0; j < 9; j++)
			edfile->INODE.i_mode &= ~(1 << j);
		
		// set file perms
		for (j = 0; j < 3; j++)
		{
			r = 8 - 3 * j;
			w = 7 - 3 * j;
			x = 6 - 3 * j;
			temp = parameter[j + 1];
			if (temp == '1' || temp == '3' || temp == '5' || temp == '7') // execute
				edfile->INODE.i_mode |= (1 << x);
			else if (temp == '2' || temp == '3' || temp == '6' || temp == '7') // write
				edfile->INODE.i_mode |= (1 << w);
			else if (temp == '4' || temp == '5' || temp == '6' || temp == '7') // read
				edfile->INODE.i_mode |= (1 << r);
		}
		printf("New Permissions: %x\n", edfile->INODE.i_mode);
		
		edfile->dirty = 1;
		iput(edfile);
	}
	else
		printf("Invalid permissions!\n");
}

// chown
void chown_file(char pathname[], char *parameter)
{
	int i = -1, ino, param;
	char *name = NULL;
	MINODE *edfile = NULL;
	complete_tokenizer(pathname, &name, &edfile);
	
	if (parameter == NULL)
	{
		printf("FAILURE!!!\n");
		return;
	}
	
	param = atoi(parameter);
	
	edfile->INODE.i_uid = param;
	edfile->dirty = 1;
	iput(edfile);
}

// touch
void touch_file(char pathname[], char *parameter)
{
	int i = -1, ino;
	char *name = NULL;
	MINODE *edfile = NULL;
	complete_tokenizer(pathname, &name, &edfile);
	//printf("edfile->ino = %d\n", edfile->ino);
	
	edfile->INODE.i_atime = time(0L);
	edfile->INODE.i_mtime = time(0L);
	edfile->dirty = 1;
	iput(edfile);
}

// open
void open_file(char pathname[], char *parameter)
{
	printf("open\n");
	/*
	1. ask for a pathname and mode to open:
         You may use mode = 0|1|2|3 for R|W|RW|APPEND

  2. get pathname's inumber:
         ino = getino(&dev, pathname);

  3. get its Minode pointer
         mip = iget(dev,ino);  

  4. check mip->INODE.i_mode to verify it's a REGULAR file and permission OK.
     (Optional : do NOT check FILE type so that we can open DIRs for RW)
     
     Check whether the file is ALREADY opened with INCOMPATIBLE type:
           If it's already opened for W, RW, APPEND : reject.
           (that is, only multiple R are OK)

  5. allocate an OpenFileTable (OFT) entry and fill in values:
         oftp = falloc();       // get a FREE OFT
         oftp->mode = mode;     // open mode 
         oftp->refCount = 1;
         oftp->inodeptr = mip;  // point at the file's minode[]

  6. Depending on the open mode 0|1|2|3, set the OFT's offset accordingly:

      switch(mode){
         case 0 : oftp->offset = 0; 
                  break;
         case 1 : truncate(mip);        // W : truncate file to 0 size
                  oftp->offset = 0;
                  break;
         case 2 : oftp->offset = 0;    // RW does NOT truncate file
                  break;
         case 3 : oftp->offset =  mip->INODE.i_size;  // APPEND mode
                  break;
         default: printf("invalid mode\n");
                  return(-1);
      }

   7. find the SMALLEST i in running PROC's fd[ ] such that fd[i] is NULL
      Let running->fd[i] point at the OFT entry

   8. update INODE's time field. 
      for W|RW|APPEND mode : mark Minode[] dirty

   9. return i as the file descriptor
	*/
}

// close
void close_file(char pathname[], char *parameter)
{
	printf("close\n");
	/*
  1. verify fd is within range.

  2. verify running->fd[fd] is pointing at a OFT entry

  3. The following code segments should be fairly obvious:
     oftp = running->fd[fd];
     running->fd[fd] = 0;
     oftp->refCount--;
     if (oftp->refCount > 0) return 0;

     // last user of this OFT entry ==> dispose of the Minode[]
     mip = oftp->inodeptr;
     iput(mip);

     fdalloc(oftp);   (optional, refCount==0 says it's FREE)
     return 0; 

	*/
}

// lseek
void lseek_file(char pathname[], char *parameter)
{
	printf("lseek\n");
	/*
	  From fd, find the OFT entry. 

  change OFT entry's offset to position but make sure NOT to over run
  either end of the file.

  return originalPosition
	*/
}

// read
void read_file(char pathname[], char *parameter)
{
	printf("read\n");
	/*
	  Preparations:
   ask for a fd  and  nbytes to read
   verify that fd is indeed opened for READ or RW

   return(myread(fd, buf, nbytes));
	*/
}

// write
void write_file(char pathname[], char *parameter)
{
	printf("write\n");
	/*
	  1. Preprations:
     ask for a fd   and   a text string to write.

  2. verify fd is indeed opened for W or RW or APPEND mode

  3. copy the text string into a buf[] and get its length as nbytes.

     return(mywrite(fd, buf, nbytes));
	*/
}

// cp
void cp_file(char pathname[], char *parameter)
{
	printf("cp\n");
	/*
	1. fd = open src for READ;
2. gd = open dst for W|CREAT; In the project, you may have to creat the dst file
                              first, then open it for WRITE, 
                              OR if open for WRITE fails, creat it and then open
                              it for WRITE.
3. while( n=read(fd, buf[ ], 1024) ){
       write(gd, buf, n);
	*/
}

// mv
void mv_file(char pathname[], char *parameter)
{
	printf("mv\n");
	/*
	
1. verify src exists; get its INODE in ==> you know its dev
2. check whether src is on the same dev as src
              CASE 1: same dev:
3. Hard link dst with src (i.e. same INODE number)
4. unlink dst (i.e. rm dst name from its parent directory and reduce INODE's
               link count by 1).
                
              CASE 2: not the same dev:
3. cp src to dst
4. unlink src
	*/
}

// mount
void mount(char pathname[], char *parameter)
{
	printf("mount\n");
	/*
	
1. Ask for filesys (a pathname) and mount_point (a pathname also).
   If mount with no parameters: display current mounted filesystems.

2. Check whether filesys is already mounted: 
   (you may store the name of mounted filesys in the MOUNT table entry). 
   If already mounted, reject;
   else: allocate a free MOUNT table entry (whose dev == 0 means FREE).

3. open filesys for RW; use its fd number as the new DEV;
   Check whether it's an EXT2 file system: if not, reject.

4. find the ino, and then the minode of mount_point:
    call  ino  = get_ino(&dev, pathname);  to get ino:
    call  mip  = iget(dev, ino);           to load its inode into memory;    

5. Check mount_point is a DIR.  
   Check mount_point is NOT busy (e.g. can't be someone's CWD)

6. Record new DEV in the MOUNT table entry;

   (For convenience, store the filesys name in the Mount table, and also
                     store its ninodes, nblocks, etc. for quick reference)

7. Mark mount_point's minode as being mounted on and let it point at the
   MOUNT table entry, which points back to the mount_point minode.

8. return 0;
	*/
}

// unmount
void umount(char pathname[], char *parameter)
{
	printf("unmount\n");
	/*
	1. Search the MOUNT table to check filesys is indeed mounted.

2. Check whether any file is still active in the mounted filesys;
      e.g. someone's CWD or opened files are still there,
   if so, the mounted filesys is BUSY ==> cannot be umounted yet.
   HOW to check?      ANS: by checking all minode[].dev

3. Find the mount_point's inode (which should be in memory while it's mounted 
   on).  Reset it to "not mounted"; then 
         iput()   the minode.  (because it was iget()ed during mounting)

4. return(0);
	*/
}

// menu
void menu()
{
	printf("Available Commands:\n");
	printf(" - menu: Displays this help menu.\n");
	printf(" - mkdir <name>: Creates a directory with the name.\n");
	printf(" - cd [path]: Changes the directory.\n");
	printf(" - pwd: Prints the cwd.\n");
	printf(" - ls [path]: lists all files in the directory/cwd.\n");
	printf(" - rmdir <name>: Removes the directory with the name.\n");
	printf(" - creat <name>: Creates a file with the name.\n");
	printf(" - link: Copies a file.\n");
	printf(" - unlink: Unlinks a file.\n");
	printf(" - symlink: Creates a symbolic link of a file.\n");
	printf(" - rm: Removes a file.\n");
	printf(" - chmod: Changes the file permissions of a file.\n");
	printf(" - chown: Changes the owner of a file.\n");
	printf(" - stat: Print file info.\n");
	printf(" - touch: touch a file.\n");
	printf(" - open: .\n");
	printf(" - close: .\n");
	printf(" - read: .\n");
	printf(" - write: .\n");
	printf(" - lseek: .\n");
	printf(" - cat: .\n");
	printf(" - cp: .\n");
	printf(" - mv: .\n");
	printf(" - mount: .\n");
	printf(" - umount: .\n");
	printf(" - quit: Exits the program.\n");
	printf(" - exit: Exits the program.\n");
}

// quit
void quit()
{
	int i;
	for(i = 0; i < NMINODES; i++)
	{
		if(minode[i]->refCount > 0 && minode[i]->dirty == 1)
		{
			iput(minode[i]);
			minode[i] = 0;
		}
	}
	exit(0);
}

// wrong command
void wrongcmd()
{
	printf("Invalid command!\n");
}