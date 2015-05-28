#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>
#include <time.h>

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

int block_size, inode_table, inode_size, first_block, inodes_per_block;
int inode_bitmap, block_bitmap, block_offset, inode_count, inode_offset;

int get_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)(blk*1024), 0);
    read(fd, buf, 1024);
}

int super(int fd)
{
  char buf[1024];

  // read SUPER block at byte offset 1024
  get_block(fd, 1, buf);
  sp = (SUPER *)buf;

  // print out fields of SUPER block
  printf("\n******************* superblock *******************\n");
  printf("s_inodes_count \t\t=\t%d\n", sp->s_inodes_count);
  inode_size = 1024 << sp->s_inodes_count;
  printf("s_blocks_count \t\t=\t%d\n", sp->s_blocks_count);
  block_size = 1024 << sp -> s_log_block_size;
  printf("s_free_inodes_count \t=\t%d\n", sp->s_free_inodes_count);
  printf("s_free_blocks_count \t=\t%d\n", sp->s_free_blocks_count);
  printf("s_log_block_size \t=\t%d\n", sp -> s_log_block_size);
  printf("s_blocks_per_group \t=\t%d\n", sp->s_blocks_per_group);
  printf("s_inodes_per_group \t=\t%d\n", sp->s_inodes_per_group);
  printf("s_mnt_count \t\t=\t%d\n", sp->s_mnt_count);
  printf("s_max_mnt_count \t=\t%d\n", sp->s_max_mnt_count);
  printf("s_magic \t\t=\t%x\n", sp->s_magic);
  printf("s_mtime \t\t=\t%s", ctime(&sp));
  printf("s_inode_size \t\t=\t%d\n", sp->s_inode_size);
  printf("\n");
}

// Where group descriptor gets its wonderful information!
int gd (int fd){
	char buf[1024];
	
	// read GD block at byte offset 1024
	get_block(fd, 2, buf);
	gp = (GD *)buf;
	
	// check EXT 2 FS magic number:
	printf("\n**************** group descriptor ****************\n");
	printf("bg_block_bitmap \t=\t%d\n", gp->bg_block_bitmap);
	block_bitmap = gp->bg_block_bitmap;
	printf("bg_inode_bitmap \t=\t%d\n", gp->bg_inode_bitmap);
	inode_bitmap = gp->bg_inode_bitmap;
	printf("bg_inode_table \t\t=\t%d\n", gp->bg_inode_table);
	inode_table = gp->bg_inode_table;
	printf("bg_free_blocks_count \t=\t%d\n", gp->bg_free_blocks_count);
	printf("bg_free_inodes_count \t=\t%d\n", gp->bg_free_inodes_count);
	printf("bg_used_dirs_count \t=\t%d\n", gp->bg_used_dirs_count);
	printf("\n");
}

int bmap(int fd){
	char buf[1024];
	char bits[8192];
	unsigned char bitmap[block_size];
	uint i = 0, j, k;
	inode_offset = inode_bitmap * inode_size;
	get_block(fd, inode_offset, bitmap);
	printf("\n**************** bmap ****************\n");
	while (i < 1440){
		bitmap [i] = bitmap[i]/8;
		bits[j] = bitmap[i] % 8;
		
		if (buf[i] & (1<<bits[j]))
			printf("1");
		else
			printf("0");
		i++;
		if((i%8) == 0){
			printf(" ");
		}
		if ((i%80) == 0){
			printf("\n");
		}
	}
	printf("\n");
}

int imap(int fd){
	char buf[1024];
	char bits[8192];
	unsigned char bitmap[inode_size];
	uint i = 0, j, k;
	block_offset = block_bitmap * block_size;
	get_block(fd, block_offset, bitmap);
	printf("\n**************** bmap ****************\n");
	while (i < 1440){
		bitmap [i] = bitmap[i]/8;
		bits[j] = bitmap[i] % 8;
		
		if (buf[i] & (1<<bits[j]))
			printf("1");
		else
			printf("0");
		i++;
		if((i%8) == 0){
			printf(" ");
		}
		if ((i%80) == 0){
			printf("\n");
		}
	}
	printf("\n");
}

int inode(int fd){
	int i = 0, j;
	char buf[1024];
	
	//get_block(fd, block_size*inode_table + inode_size, buf);
	
	ip = (INODE *)buf;
	
	printf("\n\n**************** inode ****************\n");
	//inode_block mode uid gid size time link iblock
	for ( i = 11; i != 33; i++){
		printf("i = %d\n", i);
		get_block(fd,i,buf);
		ip = (INODE *)buf;
		printf("i_mode \t\t\t=\t%x\n", ip->i_mode);
		printf("i_uid \t\t\t=\t%d\n", ip->i_uid);
		printf("i_gid \t\t\t=\t%d\n", ip->i_gid);
		printf("i_size \t\t\t=\t%d\n", ip->i_size);
		//printf("i_atime \t\t=\t%s\n", i_atime(&ip));
		//printf("i_ctime \t\t=\t%s\n", i_ctime(&ip));
		//printf("i_mtime \t\t=\t%s\n", i_mtime(&ip));
		//printf("i_dtime \t\t=\t%s\n", i_dtime(&ip));
		printf("i_links_count \t\t=\t%d\n", ip->i_links_count);
		printf("i_block[%d] \t\t=\t%d\n", i, ip->i_block[i]);
		printf("\n");
	}
	for (j = 0; j < 15; j++)
		printf("i_block[%d] \t\t=\t%d\n", j, ip->i_block[j]);
	first_block = ip->i_block[0];
}

int dir(int fd){
	int i;
	char buf[1024];
	
	get_block(fd, block_size*first_block, buf);
	dp = (DIR *)  buf;
	
	printf("\n\n**************** directories ****************\n");
	printf("%d\n", dp -> inode);
	printf("%d\n", dp -> file_type);
	printf("%d\n", dp -> name_len);
	printf("%s\n", dp -> name);
	
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
  //inode(fd);
  //dir(fd);
}
