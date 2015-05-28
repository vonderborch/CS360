#include "type.h"

int get_block(int dev, int blk, char *buf)
{
  lseek(dev, (long)(blk*1024), 0);
  read(dev, buf, 1024);
}

int put_block(int dev, int blk, char *buf)
{
	lseek(dev, (long)(blk*1024), 0);
  write(dev, buf, 1024);
}

void tokenizer(char pathway[])
{
  char *temp;
  if(pathway[0] == '/')
    startfromroot = 1;
  pathlen = 0;
  memset(path, 0, 64);
  temp = strtok(pathway, "/");
  while(temp != NULL)
  {
      path[pathlen] = temp;
      pathlen++;
      temp = strtok(NULL, "/");
  }
}

int getino(MINODE *mp, char *name)
{
  char buf[1024];
  int i, j = 1, inodenumber, result = -7, size = 0;
  get_block(dev, mp->INODE.i_block[0], buf);
  printf("mp->INODE.i_block[0] = %d\n", mp->INODE.i_block[0]);
  dp = (DIR *)buf;
  char *cp = buf;
  while(cp < buf + 1024)
  {
    char temp[100];
    strncpy(temp, dp->name, dp->name_len);
    temp[dp->name_len] = 0;
    printf("%s = %s\n", name, temp);
    getchar();
    if(!strcmp(name, temp))
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
int findmyname(MINODE *parent, int myino, char **myname)
{
  char buf[1024];
  int i, j = 1, inodenumber, result = -7, size = 0;
  get_block(dev, parent->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  char *cp = buf;
  while(cp < buf + 1024)
  {
      char temp[100];
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      if(dp->inode == myino)
	  {
        *myname = malloc(sizeof(temp));
        strncpy(*myname, temp, strlen(temp));
        return 1;
      }
      memset(temp, 0, 100);
      cp += dp->rec_len;
      dp = (DIR *)cp;
  }
  return result;
}
int findino(MINODE *mip, int *myino, int *parentino)
{
  char buf[1024];
  int i, j = 1, inodenumber, result = -7, size = 0;
  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  char *cp = buf;
  for(i = 0; i < 2; i++)
  {
      char temp[100];
      strncpy(temp, dp->name, dp->name_len);
      temp[dp->name_len] = 0;
      if(!strcmp(".", temp))
        *myino = dp->inode;
      else if(!strcmp("..", temp))
        *parentino = dp->inode;
      memset(temp, 0, 100);
      cp += dp->rec_len;
      dp = (DIR *)cp;
  }
  return 1;
}

void init(){
	int i;
	P[0] = malloc(sizeof(PROC));
	P[1] = malloc(sizeof(PROC));
	running = malloc(sizeof(PROC));
	readQueue = malloc(sizeof(PROC));
	root = malloc(sizeof(MINODE));
	for(i = 0; i < NMINODES; i++)
	{
		minode[i] = malloc(sizeof(MINODE));
		minode[i]->refCount=0;
	}
	root = 0;
	P[0]->uid = 0;
	P[0]->pid = 1;
	P[1]->pid = 2;
	P[1]->uid = 1;
	P[0]->cwd = 0;
	P[1]->cwd = 0;
	mount_root();
	running = P[0];
	readQueue = P[1];
}

void mount_root(){
	char buf[1024];
	dev = open(device, O_RDWR);
	if (dev < 0)
	{
		printf("open %s failed\n", device);
		exit(1);
	}
	minode[0] = P[1]->cwd = P[0]->cwd = root = iget(dev, 2);
	minode[0]->dev = dev;
	minode[0]->ino = 2;
	minode[0]->refCount = 3;
	minode[0]->dirty;
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	NINODES = sp->s_inodes_count;
	NBLOCKS = sp->s_blocks_count;
	if(sp->s_magic !=  0xEF53 && sp->s_magic != 0xEF51){
		printf("%x does not equal %x or %x\n", sp->s_magic, 0xEF53, 0xEF51);
		printf("Device: %s is not Ext2\n", device);
		exit(1);
	}
}

int TST_bit(char buf[], int bit)
{
	return buf[bit/8] & (1 << (bit%8));
}
int SET_bit(char buf[], int bit)
{
	return buf[bit/8] |= (1 << (bit%8));
}
int CLR_bit(char buf[ ], int bit)
{
	return buf[bit/8] &= ~(1 << (bit%8));
}

void printdir(INODE ind)
{
  printf("INODE check:\nind.i_size = %d\nind.uid=%d\nind.i_block[0]=%d\n",ind.i_size,ind.i_uid,ind.i_block[0]);
  char buf[1024];
  int i = 0, bufsize = 1024;
  get_block(dev,ind.i_block[0], buf);
  DIR *tp = (DIR*)buf;
  char *cp = buf;
  while(cp < buf + 1024)
  {
      char temp[100];
      strncpy(temp, tp->name, tp->name_len);
      temp[tp->name_len] = 0;
      printf("%s   ",temp);
      printf("rec_len = %d, bufsize = %d\n", tp->rec_len, bufsize);
      bufsize -= tp->rec_len;
      getchar();
      i++;
      cp += tp->rec_len;
      tp = (DIR *)cp;
      memset(temp, 0, 100);
  }
  printf("\n");
}

int ialloc(int dev){
   int bitpos;
   char buf[1024];
   get_block(dev, IBITMAP, buf);
   for (bitpos=0; bitpos < NINODES; bitpos++)
   {
       if (TST_bit(buf, bitpos)==0)
	   {
           SET_bit(buf, bitpos);
           put_block(dev, IBITMAP, buf);
           decFreeInodes(dev);
           return bitpos+1;
       }
   }
   printf("FS PANIC: out of INODES\n");
   return 0;
}

int decFreeInodes(int dev)
{
	char buf[1024];
	char buf2[1024];
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_inodes_count--;
	put_block(dev, 1, buf);
	get_block(dev, 2, buf2);
	gp = (GD *)buf2;
	gp->bg_free_inodes_count--;
	put_block(dev, 2, buf2);
}
int decFreeBlocks(int dev)
{
	char buf[1024];
	char buf2[1024];
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count--;
	put_block(dev, 1, buf);
	get_block(dev, 2, buf2);
	gp = (GD *)buf2;
	gp->bg_free_blocks_count--;
	put_block(dev, 2, buf2);
}
int incFreeInodes(int dev)
{
	char buf[1024];
	char buf2[1024];
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_inodes_count++;
	put_block(dev, 1, buf);
	get_block(dev, 2, buf2);
	gp = (GD *)buf2;
	gp->bg_free_inodes_count++;
	put_block(dev, 2, buf2);
}
int incFreeBlocks(int dev)
{
	char buf[1024];
	char buf2[1024];
	get_block(dev, 1, buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count++;
	put_block(dev, 1, buf);
	get_block(dev, 2, buf2);
	gp = (GD *)buf2;
	gp->bg_free_blocks_count++;
	put_block(dev, 2, buf2);
}

int balloc(int dev){
	int bitpos;
	char buf[1024];
	get_block(dev, BBITMAP, buf);
	for (bitpos=0; bitpos < NBLOCKS; bitpos++)
	{
	   if (TST_bit(buf, bitpos)==0)
	   {
		   SET_bit(buf, bitpos);
		   put_block(dev, BBITMAP, buf);
		   decFreeInodes(dev);
		   return bitpos+1;
	   }
	}
	printf("FS PANIC: out of INODES\n");
	return 0;
}

int idalloc(int dev, int ino)
{
	int i;  
	char buf[BLOCK_SIZE];
	get_block(dev, IBITMAP, buf);
	CLR_bit(buf, ino-1);
	put_block(dev, IBITMAP, buf);
	incFreeInodes(dev);
}
int bdalloc(int dev, int ino){
	int i;  
	char buf[BLOCK_SIZE];
	get_block(dev, BBITMAP, buf);
	CLR_bit(buf, ino-1);
	put_block(dev, BBITMAP, buf);
	incFreeBlocks(dev);
}

int complete_tokenizer(char pathname[], char **child, MINODE **pip)
{
	int device = 0, counter = 0, ino = 0, r;
	char *parent[63] = {NULL};
	tokenizer(pathname);
	for(counter = 0; counter < pathlen-1; counter++)
	{
		parent[counter] = malloc(sizeof(path[counter]));
		strncpy(parent[counter], path[counter], strlen(path[counter]));
		printf("parent[%d] = %s\n", counter, parent[counter]);
	}
	*child = malloc(sizeof(path[counter]));
	strncpy(*child, path[counter], strlen(path[counter]));
	if(startfromroot)
		device = root->dev;
	else
		device = running->cwd->dev;
	if(parent[0] == NULL && startfromroot == 0)
	{
		*pip = running->cwd;
		(*pip)->refCount++;
		return 1;
	}
	else if(parent[0] == NULL && startfromroot == 1)
	{
		*pip = root;
		(*pip)->refCount++;
		return 1;
	}
	else
	{
		counter = 0;
		printf("there is a path\n");
		if(startfromroot == 0)
			*pip = running->cwd;
		else
			*pip = root;
		while(parent[counter] != NULL)
		{
			ino = getino(*pip, parent[counter]);
			if(ino == -7)
			{
				printf("Directory %s does not exist\n", parent[counter]);
				return -1;
			}
			*pip = iget(dev,ino);
			if(!S_ISDIR((*pip)->INODE.i_mode))
			{
				printf("%s is not a Directory\n", parent[counter]);
				return -2;
			}
			counter++;
		}
	}
	memset(parent, 0, 63);
	(*pip)->refCount++;
	return 1;
}

MINODE *iget(dev, ino)
{
	int i; MINODE *mip;
	for (i=0; i < NMINODES; i++)
	{
		mip = minode[i];
		if (mip->refCount)
		{
			if (mip->dev==dev && mip->ino==ino)
			{
				mip->refCount++;
				printf("minode[%d]->refCount incremented\n", i);
				return mip;
			}
		}
	}
	char buf[1024];
	ipos = (ino - 1)/8 + INODE_START_POS;
	offset = (ino -1) % 8;
	printf("ipos = %d and offset = %d\n", ipos, offset);
	get_block(dev, ipos, buf);
	INODE *ip = (INODE *)buf + offset;
	for(i = 0; i < NMINODES; i++)
	{
		printf("minode[%d]->refCount = %d\n", i, minode[i]->refCount);
		if(minode[i]->refCount == 0)
		{
			printf("using the #%d minode from the array\n", i);
			mip = minode[i];
			minode[i]->refCount++;
			break;
		}
	}
	mip->INODE = *ip;
	mip->refCount = 1;
	mip->dev = dev;
	mip->ino = ino;
	mip->dirty = 0;
	return mip;
}

int iput(MINODE *mip){
	char buf[1024];
	mip->refCount--;
	if (mip->refCount == 0)
		return;
	if (mip->dirty == 0)
		return;
	int ino = getino(mip, ".");
	ipos = (ino - 1)/8 + INODE_START_POS;
	offset = (ino -1) % 8;
	printf("ipos = %d and offset = %d\n", ipos, offset);
	get_block(dev, ipos, buf);
	INODE *ip = (INODE*)buf + offset;
	memcpy(ip, &mip->INODE, sizeof(INODE));
	put_block(dev, ipos, buf);
	mip->dirty = 0;
	return 1;
}

void new_Minode(int ino, int bnum, int fod)
{
	int i;
	MINODE *mip = iget(dev,ino);
	if(fod == DIRECTORY)
	{
		mip->INODE.i_mode = 0x41ED;
		printf("dir\n");
	}
	else
	{
		mip->INODE.i_mode = 0x81A4;
		printf("file\n");
	}
	printf("mip->INODE.i_mode = %x\n", mip->INODE.i_mode);
	mip->INODE.i_uid  = running->uid;
	mip->INODE.i_gid =  running->gid;
	mip->INODE.i_size = 1024;
	mip->INODE.i_links_count = 2;
	mip->INODE.i_atime=mip->INODE.i_ctime=mip->INODE.i_mtime = time(0L); 
	mip->INODE.i_blocks = 2;
	mip->dirty = 1;
	for (i=0; i < 15; i++)
		mip->INODE.i_block[i] = 0;
	mip->INODE.i_block[0] = bnum;   
	iput(mip);
}
  
void new_directory(MINODE *pip, int ino, int bnum, char *buf)
{
	char *cp;
	DIR *dirp = (DIR *)buf;
	dirp->inode = ino;
	dirp->name_len = 1;
	dirp->rec_len = 12;
	strncpy(dirp->name, ".", 1);
	cp = buf + 12;
	dirp = (DIR *)cp;
	dirp->inode = pip->ino;
	dirp->name_len = 2;
	dirp->rec_len = BLOCK_SIZE - 12;
	strncpy(dirp->name, "..", 2);
	put_block(pip->dev, bnum, buf);
}

int my_mkdir(MINODE *pip, char *name)
{
	int n = 0, fullen = 0;
	int inumber = ialloc(pip->dev);
	printf("inumber = %d\n",inumber);
	if(inumber == 2)
	{
		inumber = ialloc(pip->dev);
		printf("inumber = %d\n",inumber);
	}
	int bnumber = balloc(pip->dev);
	new_Minode(inumber, bnumber, 2);
	char buf[1024] = {NULL}, buf2[1024] = {NULL};
	new_directory(pip, inumber, bnumber, buf);
	get_block(pip->dev, pip->INODE.i_block[0], buf2);
	DIR *tp = (DIR*)buf2;
	char *cp = buf2;
	while(cp < buf2 + 1024)
	{
		tp = (DIR *)cp;
		cp += tp->rec_len;
	}
	cp-=tp->rec_len;
	n = strlen(name);
	fullen = tp->rec_len;
	int need_length = 4*((8 + n + 3)/4);
	int ideal_length= 4*((8 + tp->name_len + 3)/4);
	printf("%d - %d >= %d\n", fullen, ideal_length, need_length);
	if(fullen - ideal_length >= need_length)
	{
		tp->rec_len = ideal_length;
		fullen -= ideal_length;
		printf("tp->reclen = %d\n", tp->rec_len);
		cp += ideal_length;
		tp = (DIR*)cp;
		tp->rec_len = fullen;
		tp->name_len = n;
		tp->inode = inumber;
		strncpy(tp->name, name, n);
		printf("size of dir = %d\n", sizeof(DIR));
	}
	else
	{
		char buffer[1024] = {NULL};
		printf("it didn't fit\n");
		bnumber = balloc(pip->dev);
		DIR *dip = (DIR*)buffer;
		dip->rec_len = BLOCK_SIZE;
		dip->name_len = n;
		strncpy(dip->name, name, n);
		put_block(pip->dev, bnumber, buffer);
	}
	put_block(pip->dev, pip->INODE.i_block[0], buf2);
	pip->INODE.i_atime = time(0L);
	return 1;
}

int my_rmdir(MINODE *pip, char *name)
{
	printf("rmdir\n");
	int checker, reclen, savelen, prevlen;
	char buf[1024] = {NULL};
	get_block(dev, pip->INODE.i_block[0], buf);
	char *cp = buf;
	char *ocp = buf;
	DIR *tp = (DIR*)buf;
	while(cp < &buf[1024])
	{
		char temp[100];
		strncpy(temp, tp->name, tp->name_len);
		temp[tp->name_len] = 0;
		if(!strcmp(temp,name))
		{
			reclen = tp->rec_len;
			savelen = reclen;
			printf("reclen = %d\n", reclen);
			if(cp+savelen >= &buf[1024])
			{
				cp -= prevlen;
				tp = (DIR*)cp;
				printf("tp->reclen = %d\n",tp->rec_len);
				tp->rec_len += savelen;
				put_block(dev, pip->INODE.i_block[0], buf);
				pip->dirty = 1;
				return 1;
			}
			while(cp < &buf[1024])
			{
				printf("loop?\n");
				memset(temp, 0, 100);
				strncpy(temp, tp->name, tp->name_len);
				temp[tp->name_len] = 0;
				printf("%s\n",temp);
				cp += savelen;
				tp = (DIR*)cp;
				savelen = tp->rec_len;
				memmove(ocp, cp, savelen);
				if(cp+savelen >= &buf[1024])
				{
				  tp = (DIR*)ocp;
				  tp->rec_len += reclen;
				  break;
				}
				ocp += savelen;
			}
			put_block(dev, pip->INODE.i_block[0], buf);
			pip->dirty = 1;
			return 1;
		}
		prevlen = tp->rec_len;
		ocp += tp->rec_len;
		cp += tp->rec_len;
		tp = (DIR *)cp;
		memset(temp, 0, 100);
	}
	printf("desired directory/file %s doesn't exist in declared directory\n", name);
	return -1;
}

int checkdir(MINODE *mip)
{
	int counter = 0, i;
	char buf[1024] = {NULL}, temp2[100] = {NULL}, buf2[1024] = {NULL};
	get_block(mip->dev, mip->INODE.i_block[0], buf2);
	if(S_ISDIR(mip->INODE.i_mode))
	{
		if(mip->INODE.i_links_count == 2)
		{
			DIR *tp = (DIR*)buf2;
			char *cp = buf2;
			while(cp < buf2 + 1024)
			{
				if(counter > 2)
				{
					printf("can't delete directories that aren't empty\n");
					return -2;
				}
				cp += tp->rec_len;
				tp = (DIR *)cp;
				counter++;
			}
		}
		else
		{
			printf("can't delete directories that aren't empty\n");
			return -2;
		}
	}
	else
	{
		printf("use rm to remove a file\n");
		return -1;
	}
	return 1;
}

int my_mkfile(MINODE *pip, char *name)
{
	int n = 0, fullen = 0;
	int inumber = ialloc(pip->dev);
	printf("inumber = %d\n",inumber);
	if(inumber == 2)
	{
		inumber = ialloc(pip->dev);
		printf("inumber = %d\n",inumber);
	}
	int bnumber = balloc(pip->dev);
	new_Minode(inumber, bnumber, 1);
	char temp2[100] = {NULL}, buf2[1024] = {NULL};
	get_block(pip->dev, pip->INODE.i_block[0], buf2);
	DIR *tp = (DIR*)buf2;
	char *cp = buf2;
	while(cp < buf2 + 1024)
	{
		tp = (DIR *)cp;
		cp += tp->rec_len;
	}
	n = strlen(name);
	fullen = tp->rec_len;
	int need_length = 4*((8 + n + 3)/4);
	int ideal_length= 4*((8 + tp->name_len + 3)/4);
	printf("%d - %d >= %d\n", fullen, ideal_length, need_length);
	if(fullen - ideal_length >= need_length)
	{
		cp-=tp->rec_len;
		tp->rec_len = ideal_length;
		fullen -= ideal_length;
		printf("tp->reclen = %d\n", tp->rec_len);
		cp += ideal_length;
		tp = (DIR*)cp;
		tp->rec_len = fullen;
		tp->name_len = n;
		tp->inode = inumber;
		strncpy(tp->name, name, n);
		printf("size of dir = %d\n", sizeof(DIR));
	}
	else
	{
		char buffer[1024] = {NULL};
		printf("it didn't fit\n");
		bnumber = balloc(pip->dev);
		DIR *dip = (DIR*)buffer;
		dip->rec_len = BLOCK_SIZE;
		dip->name_len = n;
		strncpy(dip->name, name, n);
		put_block(pip->dev, bnumber, buffer);
	}
	put_block(pip->dev, pip->INODE.i_block[0], buf2);
	pip->INODE.i_atime = time(0L);
}

MINODE *helperfunctionalpha(char *pather)
{
	MINODE *mino = NULL;
	int c, ino = 0;
	tokenizer(pather);
	if(startfromroot)
		mino = root;
	else
		mino = running->cwd;
	for(c = 0; c < pathlen; c++)
	{
		ino = getino(mino, path[c]);
		printf("ino = %d\n", ino);
		if(ino == -7)
		{
		  printf("file %s does not exist\n", path[c]);
		  iput(mino);
		  return -1;
		}
		mino = iget(dev,ino);
		printf("mino address =%d\n", mino);
		printf("c = %d, pathlen = %d\n", c, pathlen);
		if(c == pathlen - 1)
		{
			if(!S_ISREG(mino->INODE.i_mode))
			{
				printf("mino->ino = %d\n", mino->ino);
				printf("can't link a dir\n");
			}
		}
		else if(!S_ISDIR(mino->INODE.i_mode))
		{
			iput(mino);
			return -1;
		}
	}
	return mino;
}

int linkit(MINODE *first, MINODE *second, char *orig,  char *name)
{
	int n = 0, fullen = 0;
	int inumber = getino(first, orig);
	printf("inumber = %d\n");
	int bnumber = balloc(first->dev);
	new_Minode(inumber, bnumber, 1);
	char temp2[100] = {NULL}, buf2[1024] = {NULL};
	get_block(second->dev, second->INODE.i_block[0], buf2);
	DIR *tp = (DIR*)buf2;
	char *cp = buf2;
	while(cp < buf2 + 1024)
	{
		tp = (DIR *)cp;
		cp += tp->rec_len;
	}
	n = strlen(name);
	fullen = tp->rec_len;
	int need_length = 4*((8 + n + 3)/4);
	int ideal_length= 4*((8 + tp->name_len + 3)/4);
	printf("%d - %d >= %d\n", fullen, ideal_length, need_length);
	if(fullen - ideal_length >= need_length)
	{
		cp-=tp->rec_len;
		tp->rec_len = ideal_length;
		fullen -= ideal_length;
		printf("tp->reclen = %d\n", tp->rec_len);
		cp += ideal_length;
		tp = (DIR*)cp;
		tp->rec_len = fullen;
		tp->name_len = n;
		strncpy(tp->name, name, n);
		printf("size of dir = %d\n", sizeof(DIR));
	}
	else
	{
		char buffer[1024] = {NULL};
		printf("it didn't fit\n");
		bnumber = balloc(second->dev);
		DIR *dip = (DIR*)buffer;
		dip->rec_len = BLOCK_SIZE;
		dip->name_len = n;
		strncpy(dip->name, name, n);
		put_block(second->dev, bnumber, buffer);
	}
	put_block(second->dev, second->INODE.i_block[0], buf2);
	second->INODE.i_atime = time(0L);
}

void my_truncate(MINODE *mip)
{
/*
  1. release mip->INODE's data blocks;
     a file may have 12 direct blocks, 256 indirect blocks and 256*256
     double indirect data blocks. release them all.
  2. update INODE's time field

  3. set INODE's size to 0 and mark Minode[ ] dirty */
}