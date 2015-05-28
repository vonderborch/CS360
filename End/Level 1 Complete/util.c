#include "functions.h"
#include "type.h"
#include "util.h"

// read the block of data from the file device (fd) into the buffer (buf).
void get_block(int fd,int block, char *buf)
{
	lseek(fd,(long)(BLOCK_SIZE*block),0);
	read(fd,buf,BLOCK_SIZE); 
}

// put the block of data from the buffer (buf) into the file device (fd)
void put_block(int fd, int block, char *buf)
{
	lseek(fd, (long)(BLOCK_SIZE*block),0);
	write(fd, buf, BLOCK_SIZE);
}

// tokenize a path into its sub-components and return the number of pieces to the path
int token_path(char *path)
{
	char *temp;
	int i = 0;
	
	temp = strtok(path, "/");
	while(temp != NULL)
	{
		name[i] = temp;
		temp = strtok(NULL, "/");
		i++;
	}
	return i;
}

// return the directory of the path
char *dirname(char *pathname)
{
	int i = strlen(pathname) - 1;
	char *dir;
	
	while(pathname[i] != '/')
		i--;
		
	if (!i)
	{
		dir = (char *)malloc(2*sizeof(char));
		strcpy(dir, "/");
	}
	else
	{
		dir = (char *)malloc((i+1)*sizeof(char));
		strncpy(dir, pathname, i);
		dir[i] = 0;
	}

	return dir;
}

// return the basename of the path
char *basename(char *pathname)
{
	int i = strlen(pathname) - 1;
	char *base;
	while(pathname[i] != '/')
		i--;
		
	base = (char *)malloc((strlen(pathname)-i)*sizeof(char));
	strcpy(base,&pathname[i+1]);
	return base;
}

// get the inumber related to the pathname
unsigned long getino(int *device, char *pathname){
	int i, pathparts;
	unsigned long inumber;
	char path[256];
	MINODE *mip;

	// absolute or local path?
	if(pathname[0] == '/')
	{
		*device = root->dev;
		inumber = root->ino;
	}
	else // cwd
	{
		*device = running->cwd->dev;
		inumber = running->cwd->ino;
	}

	// make a backup of the command path to use so we don't destroy the original
	strcpy(path,pathname);
	pathparts = token_path(path);

	// cycle through each sub-component of the path to get the end inode
	for(i = 0; i < pathparts ; i++)
	{
		mip = iget(*device, inumber); // get the minode for the current inumber
		inumber = search(mip, name[i]); // search for the next inumber based on the results

		// path dne
		if(inumber == 0)
		{
			printf("The file/directory '%s' does not exist!\n",name[i]);
			iput(mip); // cleanup
			return 0; // return that we failed :(
		}

		// path part is not a directory
		if((mip->INODE.i_mode & 0040000) != 0040000)
		{
			printf("The path part '%s' is not a directory!\n",name[i]);
			iput(mip); // cleanup
			return 0; // return that we failed :(
		}
		iput(mip); // cleanup
	}
	return inumber; // return the inumber for the query
}

// search for the specified file in this inode's directory
unsigned long search(MINODE * mip, char *filename)
{
	int i;
	char buf[BLOCK_SIZE], namebuf[256], *cp;

	// search through all *direct* blocks
	for(i = 0; i <= 11 ; i ++)
	{
		// does this block exist?
		if(mip->INODE.i_block[i])
		{
			get_block(mip->dev, mip->INODE.i_block[i], buf);
			dp = (DIR *)buf;
			cp = buf;

			while(cp < &buf[BLOCK_SIZE])
			{
				strncpy(namebuf,dp->name,dp->name_len);
				namebuf[dp->name_len] = 0;

				// have we found the correct inode?
				if(!strcmp(namebuf, filename))
					return  dp->inode;
				cp +=dp->rec_len;
				dp = (DIR *)cp;
			}
		}    
	}
	return 0;
}

// get the minode related to the specified inumber on the device
MINODE *iget(int dev, unsigned long ino){
	int i, nodeIndex, blockIndex;
	INODE *cp;
	char buf[BLOCK_SIZE];
	
	// does this minode already exist in memory?
	for(i = 0; i < NMINODES; i++)
	{
		if(minode[i].refCount)
		{
			// if it does exist, return it!
			if(minode[i].dev == dev && minode[i].ino == ino)
			{
				minode[i].refCount++;
				return &minode[i];
			}
		}
	}

	// otherwise, we need to find an empty minode slot
	for(i = 0; i < NMINODES; i++)
	{
		// have we found an empty minode slot to allocate for a new minode?
		if(minode[i].refCount == 0)
		{
			// allocate the new minode!
			nodeIndex = (ino - 1) % INODES_PER_BLOCK;// Mailman's Algorithm
			blockIndex = (ino - 1) / INODES_PER_BLOCK + INODEBLOCK; // Mailman's Algorithm
			get_block(dev,blockIndex,buf);
			cp = (INODE *)buf;
			cp+= nodeIndex;
			minode[i].INODE = *cp;
			minode[i].dev = dev;
			minode[i].ino = ino;
			minode[i].refCount = 1;
			minode[i].dirty = 0;
			return &minode[i];
		}
	}
	
	printf("ERROR: we've run out of minode slots :(\n");
	exit(0);
}

// save the minode back onto the disk
void iput(MINODE *mip)
{
	int nodeIndex,blockIndex;
	char buf[BLOCK_SIZE];

	mip->refCount--;

	// is the minode still being used or not used?
	if((mip->refCount) || (mip->dirty == 0))
		return;

	nodeIndex = (mip->ino -1 ) % INODES_PER_BLOCK; // Mailman's Algorithm
	blockIndex = (mip->ino -1) / INODES_PER_BLOCK + INODEBLOCK; // Mailman's Algorithm

	get_block(mip->dev,blockIndex, buf);
	ip = (INODE *)buf;
	ip += nodeIndex;
	*ip = mip->INODE;
	put_block(mip->dev,blockIndex,buf); // save the minode!
}

// find the file with inumber and return its name (basically same as search)
int findname(MINODE *parent, unsigned long myino, char *name)
{
	int i;
	char buf[BLOCK_SIZE], namebuf[256], *cp;

	for(i = 0; i <= 11 ; i ++)
	{
		if(parent->INODE.i_block[i] != 0)
		{
			get_block(parent->dev, parent->INODE.i_block[i], buf);
			dp = (DIR *)buf;
			cp = buf;

			while(cp < &buf[BLOCK_SIZE])
			{
				strncpy(namebuf,dp->name,dp->name_len);
				namebuf[dp->name_len] = 0;

				if(dp->inode == myino)
				{
					strcpy(name, namebuf);
					return 1; // success!
				}
				cp +=dp->rec_len;
				dp = (DIR *)cp;
			}
		}
	}
	return -1; // no success :(
}

// get the inode
int findino(MINODE *mip, unsigned long *myino, unsigned long *parentino)
{
	int i;
	char buf[BLOCK_SIZE], namebuf[256], *cp;

	get_block(mip->dev, mip->INODE.i_block[0], buf);
	dp = (DIR *)buf;
	cp = buf;
	 
	*myino = dp->inode;
	cp +=dp->rec_len;
	dp = (DIR *)cp;
	*parentino = dp->inode;	    	
	return 0;
}

// allocate an inode
unsigned long ialloc(int dev)
{
	int i, ninodes;
	char buf[BLOCK_SIZE]; // BLOCK_SIZE = block size in bytes
	SUPER *temp;

	// get total number of inodes
	get_block(dev,SUPERBLOCK,buf);
	temp = (SUPER *)buf;
	ninodes = temp->s_inodes_count;
	put_block(dev,SUPERBLOCK,buf);

	// get inode bitmap into buf
	get_block(dev, IBITMAP,buf);

	for(i = 0; i < ninodes ; i++) 
	{
		if(TST_bit(buf,i) == 0)
		{
			SET_bit(buf,i);
			put_block(dev,IBITMAP,buf); // write imap block back to disk

			// update free inode count in SUPER and GD on dev
			decFreeInodes(dev);
			return i+1;
		}
	}

	return 0; // no more free inodes :(
}

// deallocate an inode
unsigned long idealloc(int dev, unsigned long ino)
{
	int i ;
	char buf[BLOCK_SIZE];

	// get inode bitmap block
	get_block(dev, IBITMAP, buf);
	CLR_bit(buf,ino-1);

	// write buf back
	put_block(dev, IBITMAP,buf);

	// update free inode count in SUPER and GD
	incFreeInodes(dev);
}

// allocate disk block (works same as ialloc)
unsigned long balloc(int dev)
{
	int i;
	char buf[BLOCK_SIZE];
	int nblocks;
	SUPER *temp;

	// get total number of blocks
	get_block(dev,SUPERBLOCK,buf);
	temp = (SUPER *)buf;
	nblocks = temp->s_blocks_count;
	put_block(dev,SUPERBLOCK,buf);

	get_block(dev, BBITMAP,buf);

	for(i = 0; i < nblocks ; i++)
	{
		if(TST_bit(buf,i) == 0)
		{
			SET_bit(buf,i);
			put_block(dev,BBITMAP,buf);

			decFreeBlocks(dev);
			return i+1;
		}
	}
	return 0;
}

// deallocate disk block (works same as idealloc)
unsigned long bdealloc(int dev, unsigned long iblock)
{
	int i ;
	char buf[BLOCK_SIZE];

	// get inode bitmap block
	get_block(dev, BBITMAP, buf);
	CLR_bit(buf,iblock-1);

	put_block(dev, BBITMAP,buf);

	incFreeBlocks(dev);
}

// increment free inodes on device
void incFreeInodes(int dev)
{
	char buf[BLOCK_SIZE];
	get_block(dev, SUPERBLOCK, buf);
	sp = (SUPER *)buf;
	sp->s_free_inodes_count++;
	put_block(dev, SUPERBLOCK,buf);

	get_block(dev, GDBLOCK,buf);
	gp = (GD *)buf;
	gp->bg_free_inodes_count++;
	put_block(dev, GDBLOCK,buf);
}

// decrement free inodes on device
void decFreeInodes(int dev)
{
	char buf[BLOCK_SIZE];
	get_block(dev, SUPERBLOCK, buf);
	sp = (SUPER *)buf;
	sp->s_free_inodes_count--;
	put_block(dev, SUPERBLOCK,buf);

	get_block(dev, GDBLOCK,buf);
	gp = (GD *)buf;
	gp->bg_free_inodes_count--;
	put_block(dev, GDBLOCK,buf);
}

// increment free blocks on device
void incFreeBlocks(int dev)
{
	char buf[BLOCK_SIZE];
	get_block(dev, SUPERBLOCK, buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count++;
	put_block(dev, SUPERBLOCK,buf);

	get_block(dev, GDBLOCK,buf);
	gp = (GD *)buf;
	gp->bg_free_blocks_count++;
	put_block(dev, GDBLOCK,buf);
}

// decrement free blocks on device
void decFreeBlocks(int dev)
{
	char buf[BLOCK_SIZE];
	get_block(dev, SUPERBLOCK, buf);
	sp = (SUPER *)buf;
	sp->s_free_blocks_count--;
	put_block(dev, SUPERBLOCK,buf);

	get_block(dev, GDBLOCK,buf);
	gp = (GD *)buf;
	gp->bg_free_blocks_count--;
	put_block(dev, GDBLOCK,buf);
}

// test a bit
int TST_bit(char *buf, int BIT)
{
	return buf[BIT / 8] & (1 << (BIT % 8));
}

// set a bit
int SET_bit(char *buf, int BIT)
{
	return buf[BIT / 8] |= (1 << (BIT % 8));
}

// clear a bit
int CLR_bit(char *buf, int BIT)
{
	return buf[BIT / 8] &= ~(1 << (BIT % 8));
}

// print a bad path error
void patherror(char *cmdtemp)
{
	printf("%s: bad path!\n", cmdtemp);
}