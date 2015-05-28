/**********************************************************************************************
* Programmer: Christian Webber
* Group #1, Seq #19
* Class: CptS 360, Spring 2014;
* Lab 5
* File: showblock.c
*********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define BLKSIZE 1024
#define BASE_OFFSET 1024
#define BLOCK_OFFSET(block) (1024 + (block - 1) * blk_size)

// define shorter TYPES
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR; 

// define pointer variables
GD    *gp;
SUPER *sp;
INODE *ip;
DIR *dp;
char *cp, *device, *path[64];

static unsigned int blk_size = 0;

int inode_count, block_count, inode_size, block_size,
	inode_bitmap, block_bitmap, inode_table, path_parts = 0;

int main(int argc, char *argv[])
{ 
	int fd, ino = 2, i;
	char full_path[2048], backup[2048];

	// check if there are passed arguments
	if (argc > 2)
	{
		printf("\n");
		device = argv[1];
		strcpy(full_path, argv[2]);
	}
	else switch (argc)
	{
		case 2:
			printf("Pathname not given as an argument!\n");
			exit(0);
			break;
		case 1:
			printf("Disk and pathnames not given as arguments!\n");
			exit(0);
			break;
	}

	// open the device
	fd = open(device, O_RDONLY);
	if (fd < 0)
	{
		printf("Cannot open %s\n", device);
		exit(0);
	}

	// do the stuff
	printf("Opening %s, looking for %s\n", device, full_path);
	strcpy(backup, full_path);
	if (strcmp("/", backup) != 0)
		breakup_path(full_path);
	printf("n=%d", path_parts);
	for (i = 0; i < path_parts; i++)
		printf("   %s", path[i]);
	printf("\n***************************************************\n");
	super(fd);
	get_values(fd);
	print_root(fd);
	if (strcmp("/", backup) != 0)
		ino = search(fd);
	print_info(fd, ino, backup);

	// cleanup
	close(fd);
	printf("\n\n");

	return 0;
}

void breakup_path(char full_path[2048])
{
	char *temp;
	temp = strtok(full_path, "/");
	while (temp != NULL)
	{
		path[path_parts] = temp;
		path_parts++;
		temp = strtok(0, "/");
	}
}

void get_block(int fd, int blk, char buf[BLKSIZE])
{
	lseek(fd, (long)(blk * BLKSIZE), 0);
	read(fd, buf, BLKSIZE);
}

static void get_inode(int fd, int ino, INODE *inode)
{
	lseek(fd, BLOCK_OFFSET(inode_table) + (ino - 1) * sizeof(INODE), SEEK_SET);
	read(fd, inode, sizeof(INODE));
}

void super(int fd)
{
	char buf[BLKSIZE];

	// read in the SUPER block
	get_block(fd, 1, buf);
	sp = (SUPER *)buf;

	if (sp->s_magic != 0xEF53 && sp->s_magic != 0xEF51)
	{
		printf("%s is not a valid device!\n", device);
		exit(0);
	}

	// print out fields
	printf("\n******************* superblock *******************\n");
	printf("s_magic \t\t=\t%x\n", sp->s_magic);
	printf("s_inodes_count \t\t=\t%d\n", sp->s_inodes_count);
	printf("s_blocks_count \t\t=\t%d\n", sp->s_blocks_count);
	printf("s_r_blocks_count \t=\t%d\n", sp->s_r_blocks_count);
	printf("s_free_blocks_count \t=\t%d\n", sp->s_free_blocks_count);
	printf("s_free_inodes_count \t=\t%d\n", sp->s_free_inodes_count);
	printf("s_first_data_block \t=\t%d\n", sp->s_first_data_block);
	printf("s_log_block_size \t=\t%d\n", sp->s_log_block_size);
	printf("s_log_frag_size \t=\t%d\n", sp->s_log_frag_size);
	printf("s_blocks_per_group \t=\t%d\n", sp->s_blocks_per_group);
	printf("s_frags_per_group \t=\t%d\n", sp->s_frags_per_group);
	printf("s_inodes_per_group \t=\t%d\n", sp->s_inodes_per_group);
	printf("s_mtime \t\t=\t%s", ctime(&(sp->s_mtime)));
	printf("s_wtime \t\t=\t%s", ctime(&(sp->s_wtime)));
	printf("s_mnt_count \t\t=\t%d\n", sp->s_mnt_count);
	printf("s_max_mnt_count \t=\t%d\n", sp->s_max_mnt_count);
	printf("s_state \t\t=\t%d\n", sp->s_state);
	printf("s_errors \t\t=\t%d\n", sp->s_errors);
	printf("s_minor_rev_level \t=\t%d\n", sp->s_minor_rev_level);
	printf("s_lastcheck \t\t=\t%d\n", sp->s_lastcheck);
	printf("s_checkinterval \t=\t%d\n", sp->s_checkinterval);
	printf("s_creator_os \t\t=\t%d\n", sp->s_creator_os);
	printf("s_rev_level \t\t=\t%d\n", sp->s_rev_level);
	printf("s_def_resuid \t\t=\t%d\n", sp->s_def_resuid);
	printf("s_def_resgid \t\t=\t%d\n", sp->s_def_resgid);
	printf("s_first_ino \t\t=\t%d\n", sp->s_first_ino);
	printf("s_inode_size \t\t=\t%d\n", sp->s_inode_size);
}

void get_values(int fd)
{
	char buf[BLKSIZE];

	// information from the SUPER block
	inode_count = sp->s_inodes_count;
	block_count = sp->s_blocks_count;
	inode_size = sp->s_inode_size;
	block_size = sp->s_log_block_size;
	blk_size = 1024 << block_size;

	// information from the GROUP DESCRIPTER block
	get_block(fd, 2, buf);
	gp = (GD *)buf;
	printf("\n**************** group descriptor *****************\n");
	printf("bg_block_bitmap \t=\t%d\n", gp->bg_block_bitmap);
	printf("bg_inode_bitmap \t=\t%d\n", gp->bg_inode_bitmap);
	printf("bg_inode_table \t\t=\t%d\n", gp->bg_inode_table);
	printf("bg_free_blocks_count \t=\t%d\n", gp->bg_free_blocks_count);
	printf("bg_free_inodes_count \t=\t%d\n", gp->bg_free_inodes_count);
	printf("bg_used_dirs_count \t=\t%d\n", gp->bg_used_dirs_count);
	printf("bg_pad \t=\t%d\n", gp->bg_pad);
	block_bitmap = gp->bg_block_bitmap;
	inode_bitmap = gp->bg_inode_bitmap;
	inode_table = gp->bg_inode_table;

	// get the location of the first block
	ip = (INODE *)malloc(inode_size);
	lseek(fd, 1024 * inode_table + inode_size, SEEK_SET);
	read(fd, ip, inode_size);
	printf("\n******************* root inode ********************\n");
	printf("inode_block=%d\n", inode_count / 8);
	printf("mode=%x\n", ip->i_mode);
	printf("uid=%d\n", ip->i_uid);
	printf("gid=%d\n", ip->i_gid);
	printf("size=%d\n", ip->i_size);
	printf("time=%s", ctime(&(ip->i_ctime)));
	printf("link=%d\n", ip->i_links_count);
	printf("blocks=%d\n", ip->i_blocks);
	printf("i_block[0]=%d\n", ip->i_block[0]);
}

void print_root(int fd)
{
	int i, inumber;
	INODE dirnode;
	char buf[BLKSIZE], temp[256];

	get_inode(fd, 2, &dirnode);
	print_root_helper(fd, &dirnode);
	printf("***************************************************\n\n");
}

void print_root_helper(int fd, INODE *inodePtr)
{
	int i;
	char buf[BLKSIZE], temp[256];

	printf("\n****************** root directory *****************\n");
	for (i = 0; i < 12; i++)
	{
		if (inodePtr->i_block[i] == 0)
			return;

		get_block(fd, inodePtr->i_block[i], buf);
		dp = (DIR *)buf;
		cp = buf;
		printf("inode \t rec_len \t name_len \t name\n");

		while (cp < buf + BLKSIZE)
		{
			strncpy(temp, dp->name, dp->name_len);
			temp[dp->name_len] = 0;
			printf("%4d \t %4d \t\t %4d \t\t %s\n", dp->inode, dp->rec_len, dp->name_len, temp);

			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
	}
}

int search(int fd)
{
	int i, inumber;
	INODE dirnode;

	printf("******** Beginning search for the file... ********\n");
	get_inode(fd, 2, &dirnode);
	for (i = 0; i < path_parts; i++)
	{
		if (i < path_parts)
		{
			if (!S_ISDIR(dirnode.i_mode))
			{
				printf("A directory is not a file!\n");
				exit(0);
			}
		}
		inumber = sub_search(fd, &dirnode, path[i]);
		if (inumber == 0)
		{
			printf("File does not exist!\n");
			exit(0);
		}
		printf("Found %s at inode #%d!\n", path[i], inumber);

		get_inode(fd, inumber, &dirnode);
	}
	return inumber;
}

int sub_search(int fd, INODE *inodePtr, char *filename)
{
	int i;
	char buf[BLKSIZE], temp[256];

	printf("Searching for %s...\n", filename);
	for (i = 0; i < 12; i++)
	{
		if (inodePtr->i_block[i] == 0)
			return 0;
		get_block(fd, inodePtr->i_block[i], buf);
		dp = (DIR *)buf;
		cp = buf;
		printf("inode \t rec_len \t name_len \t name\n");

		while (cp < buf + BLKSIZE)
		{
			strncpy(temp, dp->name, dp->name_len);
			temp[dp->name_len] = 0;
			printf("%4d \t %4d \t\t %4d \t\t %s\n", dp->inode, dp->rec_len, dp->name_len, temp);

			if (!strcmp(filename, temp))
				return dp->inode;
			cp += dp->rec_len;
			dp = (DIR *)cp;
		}
	}
	return 0;
}

void print_info(int fd, int ino, char *name)
{
	int i, j, cycle_blocks, num_blocks, indirect[256], double_indirect[256];	
	INODE file;
	SUPER super;

	lseek(fd, BASE_OFFSET, SEEK_SET); 
	read(fd, &super, sizeof(super));
	blk_size = 1024 << super.s_log_block_size;
	get_inode(fd, ino, &file);
	num_blocks = file.i_size / BLKSIZE;
	if (num_blocks > 1)
		num_blocks++;
	else if (num_blocks == 0)
		num_blocks = 1;
	cycle_blocks = num_blocks;

	printf("***************************************************\n");
	printf("We've found %s!\n", name);
	printf("**** %s Basic Stats ****\n", name);
	printf("   mode: %x\n", file.i_mode);
	printf("   uid: %d\n", file.i_uid);
	printf("-->size: %u\n", file.i_size);
	printf("-->blocks: %u\n", num_blocks);
	printf("   access time: %s", ctime(&(file.i_atime)));
	printf("   creation time: %s", ctime(&(file.i_ctime)));
	printf("   modification time: %s", ctime(&(file.i_mtime)));
	printf("   gid: %d\n", file.i_gid);
	printf("   links count: %d\n", file.i_links_count);
	printf("   flags: %d\n", file.i_flags);

	printf("**** DISK BLOCKS ****\n");
	for (i = 0; i < 14; i++)
		printf("   block[%d]: %d\n", i, file.i_block[i]);

	printf("\n**** DIRECT BLOCKS ****\n   ");
	if (cycle_blocks > 12)
		cycle_blocks = 12;
	print_cycle(cycle_blocks, file.i_block);
	num_blocks -= cycle_blocks;
	printf("\nBlocks Remaining: %u\n", num_blocks);

	if (num_blocks > 0)
	{
		printf("**** INDIRECT BLOCKS ****\n   ");
		cycle_blocks = num_blocks;
		if (cycle_blocks > 256)
			cycle_blocks = 256;
		get_block(fd, file.i_block[12], indirect);
		print_cycle(cycle_blocks, indirect);
		num_blocks -= cycle_blocks;
		printf("\nBlocks Remaining: %u\n", num_blocks);

		if (num_blocks > 0)
		{
			printf("**** DOUBLE INDIRECT BLOCKS ****\n");
			get_block(fd, file.i_block[13], double_indirect);
			for (j = 0; j < 256; j++)
			{
				if (double_indirect[j] == 0)
					break;
				printf("---- %d ----\n   ", double_indirect[j]);
				cycle_blocks = num_blocks;
				if (cycle_blocks > 256)
					cycle_blocks = 256;
				get_block(fd, double_indirect[j], indirect);
				print_cycle(cycle_blocks, indirect);
				num_blocks -= cycle_blocks;
				printf("\nBlocks Remaining: %u\n", num_blocks);
			}
		}
	}
}

void print_cycle(int cycle_blocks, int indirect[256])
{
	int i, mod = 16;
	for (i = 0; i < cycle_blocks; i++)
	{
		printf("%d ", indirect[i]);
		if ((i + 1) % mod == 0)
			printf("\n   ");
	}
}
