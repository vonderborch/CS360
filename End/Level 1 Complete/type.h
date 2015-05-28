/*	type.h for CS360 Project             */
#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/ioctl.h>

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#ifndef BLOCK_SIZE
#define BLOCK_SIZE        1024
#endif

#define BITS_PER_BLOCK    (8*BLOCK_SIZE)
#define INODES_PER_BLOCK  (BLOCK_SIZE/sizeof(INODE))

// Block number of EXT2 FS on FD
#define SUPERBLOCK        1
#define GDBLOCK           2
#define BBITMAP           3
#define IBITMAP           4
#define INODEBLOCK        5
#define ROOT_INODE        2
#define INODE_START_POS   5

// Default dir and regular file modes
#define DIR_MODE          0040777 
#define FILE_MODE         0100644
#define SUPER_MAGIC       0xEF53
#define SUPER_USER        0

// Proc status
#define FREE              0
#define BUSY              1
#define KILLED            2

// Table sizes
#define NMINODES          100
#define NMOUNT            10
#define NPROC             10
#define NFD               10
#define NOFT              100

// DIRORFILE
#define LINK 			0
#define FILE 			1
#define DIRECTORY		2

// Color codes
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[1;34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// Open File Table
typedef struct Oft{
  int   mode, refCount;
  struct Minode *inodeptr;
  long  offset;
} OFT;

// PROC structure
typedef struct Proc{
  int   uid, pid, gid, ppid, status;
  struct Proc *parent;
  struct Minode *cwd;
  OFT   *fd[NFD];
} PROC;
      
// In-memory inodes structure
typedef struct Minode{		
  INODE    INODE;               // disk inode
  ushort   dev, refCount, dirty, mounted;
  unsigned long ino;
  struct Mount *mountptr;
} MINODE;

// Mount Table structure
typedef struct Mount{
        int    ninodes, nblocks, dev, busy;
        struct Minode *mounted_inode;
        char   name[256], mount_name[64]; 
} MOUNT;

// function table structure
typedef struct function_table{
  char *functionName;
  int (*f)();
  char *functionParameters, *functionHelp;
  int paramOptional; // 0 = no, 1 = yes, 2 = has no params
} functionTable;

// Global variables
PROC P[2];
MINODE minode[NMINODES];
int dev, sleepmode;
MINODE *root;
PROC *running, *readQueue;
char pathname[256], parameter[256], *name[256], lastcommands[16][256];

#endif 
