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

int block_size, inode_table, inode_size, first_block, inodes_per_block, inode_bitmap, block_bitmap, block_offset, inode_count, inode_offset;

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
  block_size = 1024 << sp -> s_log_block_size;
  inode_size = sp->s_inode_size;
  printf("\n**************************************\n");
}

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
	printf("\n**************************************\n");
}

int bmap(int fd){
	char buf[1024];
	char bits[8192];
	unsigned char bitmap[block_size];
	uint i = 0, j, k;
	get_block(fd, 8, bitmap);
	printf("\n**************** bmap ****************\n");
	while (i < 1040){ // works up to i<1068, i<1069+ makes it loop infinitely since 32-bit
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
	printf("\n**************************************\n");
}

int imap(int fd){
	char buf[1024];
	char bits[8192];
	unsigned char bitmap[inode_size];
	uint i = 0, j, k;
	get_block(fd, 9, buf);
	printf("\n**************** imap ****************\n");
	while (i < 160){ 
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
	printf("\n**************************************\n");
}

int inode(int fd){
	int i = 0, j;
	char buf[1024];
	
	get_block(fd, block_size*inode_table + inode_size, buf);
	
	ip = (INODE *)buf;
	
	printf("\n\n**************** inode ****************\n");
	printf("inode_block=%d\n", inode_count / 8);

	printf("mode=%x\n", ip->i_mode);
	printf("uid=%d\n", ip->i_uid);
	printf("gid=%d\n", ip->i_gid);
	printf("size=%d\n", ip->i_size);
	printf("time=%s", ctime(&(ip->i_ctime)));
	printf("link=%d\n", ip->i_links_count);
	printf("i_block[0]=%d\n", ip->i_block[i]);
	printf("\n**************************************\n");
}

int dir(int fd){
	char buf[1024];

	DIR *dpp = (DIR *)buf;
	char *cpp = buf;	

	get_block(fd, 10, buf);
	printf("\n**************** directories ****************\n");
		printf("%d %d %s", dpp->inode, dpp->file_type, dpp->name);
	/*while (cpp < buf + block_size)
	{
		printf("%d %d %s", dpp->inode, dpp->file_type, dpp->name);
		cpp += dpp->rec_len;
		dpp = (DIR *)cpp;
	}*/
	printf("\n**************************************\n");
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
