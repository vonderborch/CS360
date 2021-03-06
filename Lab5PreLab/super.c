#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>
#include <time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

// define shorter TYPES
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

// define pointer variables
GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

int inode_count, block_count, inode_size, block_size, block_bitmap, inode_bitmap, inode_table, first_block;

int get_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)(blk*1024), 0);
    read(fd, buf, 1024);
}

int super(int fd)
{
	char buf[1024];

	get_block(fd, 1, buf);
	sp = (SUPER *)buf;

	printf("\n******************* superblock *******************\n");
	printf("s_inodes_count \t\t=\t%d\n", sp->s_inodes_count);
	printf("s_blocks_count \t\t=\t%d\n", sp->s_blocks_count);
	printf("s_free_inodes_count \t=\t%d\n", sp->s_free_inodes_count);
	printf("s_free_blocks_count \t=\t%d\n", sp->s_free_blocks_count);
	printf("s_log_block_size \t=\t%d\n", sp -> s_log_block_size);
	printf("s_blocks_per_group \t=\t%d\n", sp->s_blocks_per_group);
	printf("s_inodes_per_group \t=\t%d\n", sp->s_inodes_per_group);
	printf("s_mnt_count \t\t=\t%d\n", sp->s_mnt_count);
	printf("s_max_mnt_count \t=\t%d\n", sp->s_max_mnt_count);
	printf("s_magic \t\t=\t%x\n", sp->s_magic);
	printf("s_mtime \t\t=\t%s", ctime(&(sp->s_mtime)));
	printf("s_inode_size \t\t=\t%d\n", sp->s_inode_size);
	inode_count = sp->s_inodes_count;
	block_count = sp->s_blocks_count;
	block_size = 1024 << sp -> s_log_block_size;
	inode_size = sp->s_inode_size;
	printf("\n**************************************\n");
}

int gd (int fd)
{
	char buf[1024];
	
	get_block(fd, 2, buf);
	gp = (GD *)buf;
	
	printf("\n**************** group descriptor ****************\n");
	printf("bg_block_bitmap \t=\t%d\n", gp->bg_block_bitmap);
	printf("bg_inode_bitmap \t=\t%d\n", gp->bg_inode_bitmap);
	printf("bg_inode_table \t\t=\t%d\n", gp->bg_inode_table);
	printf("bg_free_blocks_count \t=\t%d\n", gp->bg_free_blocks_count);
	printf("bg_free_inodes_count \t=\t%d\n", gp->bg_free_inodes_count);
	printf("bg_used_dirs_count \t=\t%d\n", gp->bg_used_dirs_count);
	block_bitmap = gp->bg_block_bitmap;
	inode_bitmap = gp->bg_inode_bitmap;
	inode_table = gp->bg_inode_table;
	printf("\n**************************************\n");
}

int bmap(int fd)
{
	unsigned char bitmap[block_size];
	int i, j, k;

	get_block(fd, block_bitmap, bitmap);
	printf("\n**************** bmap ****************\n");
	printf("block_bitmap: %d\n", block_bitmap);
	printf("block_size: %d\n", block_size);
	for (i = 0; i <= block_count; i++)
	{
		j = bitmap[i]/8;
		k = bitmap[i]%8;
		if (bitmap[j] & (1<<k))
			printf("1");
		else
			printf("0");
		if (i > 0)
		{
			if ((i%8) == 0)
				printf(" ");
			if ((i%80) == 0)
				printf("\n");
		}
		j++;
	}
	printf("\n**************************************\n");
}

int imap(int fd)
{
	char buf[1024];
	unsigned char bitmap[inode_size];
	int i, j, k;
	get_block(fd, inode_bitmap, buf);
	printf("\n**************** imap ****************\n");
	printf("inode_bitmap: %d\n", inode_bitmap);
	printf("inode_size: %d\n", inode_size);
	for (i = 0; i <= inode_count; i++)
	{
		j = bitmap[i]/8;
		k = bitmap[i]%8;
		if (bitmap[j] & (1<<k))
			printf("1");
		else
			printf("0");
		if (i > 0)
		{
			if ((i%8) == 0)
				printf(" ");
			if ((i%80) == 0)
				printf("\n");
		}
		j++;
	}
	printf("\n**************************************\n");
}

int inode(int fd)
{
	int i = 0, j;
	char buf[1024];

	ip = (INODE *)malloc(inode_size);

	lseek(fd,1024*inode_table+inode_size,SEEK_SET);
	read(fd,ip,inode_size);
	
	printf("\n\n**************** inode ****************\n");
	printf("inode_block=%d\n", inode_count / 8);

	printf("mode=%x\n", ip->i_mode);
	printf("uid=%d\n", ip->i_uid);
	printf("gid=%d\n", ip->i_gid);
	printf("size=%d\n", ip->i_size);
	printf("time=%s", ctime(&(ip->i_ctime)));
	printf("link=%d\n", ip->i_links_count);
	printf("i_block[0]=%d\n", ip->i_block[0]);
	first_block = ip->i_block[0];
	printf("\n**************************************\n");
}

int dir(int fd)
{
	unsigned int size = 0;
	unsigned char buf[1024];

	lseek(fd, 1024*first_block, SEEK_SET);
	read(fd, &buf, 1024);

	dp = (DIR *)buf;

	printf("\n**************** directories ****************\n");
	while (size < inode_size)
	{
		printf("%d %d %d %s\n", dp->inode, dp->file_type, dp->name_len, dp->name);
		dp = (void *)dp + dp->rec_len;
		size += dp->rec_len;
	}
}

char *device = "mydisk";
main(int argc, char *argv[])
{ 
  int fd;

  if (argc > 1)
     device = argv[1];

  fd = open(device, O_RDONLY);
  if (fd < 0){
     printf("open %s failed\n", device);
     exit(1);
  }
  super(fd);
  gd(fd);
  bmap(fd);
  imap(fd);
  inode(fd);
  dir(fd);
}
