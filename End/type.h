/*	type.h for CS360 Project             */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ext2_fs.h>
#include <libgen.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

#define BLOCK_SIZE        1024
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

// Default dir and regulsr file modes
#define DIR_MODE          0040777 
#define FILE_MODE         0100644
#define SUPER_MAGIC       0xEF53
#define SUPER_USER        0

// Proc status
#define FREE              0
#define READY             1
#define RUNNING           2

// Table sizes
#define NMINODES         100
#define NMOUNT            10
#define NPROC             10
#define NFD               10
#define NOFT             100

// DIRORFILE
#define FILE 			1
#define DIRECTORY		2

// Open File Table
typedef struct oft{
  int   mode;
  int   refCount;
  struct minode *inodeptr;
  int   offset;
} OFT;

// PROC structure
typedef struct proc{
  int   uid;
  int   pid;
  int   gid;
  int   ppid;
  struct proc *parent;
  int   status;
  struct minode *cwd;
  OFT   *fd[NFD];
} PROC;
      
// In-memory inodes structure
typedef struct minode{		
  INODE    INODE;               // disk inode
  int      dev, ino;
  int      refCount;
  int      dirty;
  int      mounted;
  struct mount *mountptr;
} MINODE;

// Mount Table structure
typedef struct mount{
        int    ninodes;
        int    nblocks;
        int    dev;
        MINODE *mounted_inode;
        char   name[256]; 
        char   mount_name[64];
} MOUNT;

// globals
PROC *P[2] = {NULL};
MINODE *minode[100] = {NULL} ;
MINODE *root = NULL;
PROC *running = NULL;
PROC *readQueue = NULL;
int dev, offset = 1, ipos = INODE_START_POS, pathlen = 0, firstrun = 0, NINODES, NBLOCKS, startfromroot = 0;
char pathname[64], command[64], parameter[64];
char *device = "mydisk";
const char *path[64];

// functions
int findino(MINODE *mip, int *myino, int *parentino);
int findmyname(MINODE *parent, int myino, char **myname);
int iput(MINODE *mip);
MINODE *iget(int dev, int ino);
int getino(MINODE *mp, char *name);
void tokenizer(char pathway[]);
int put_block(int dev, int blk, char *buf);
int get_block(int dev, int blk, char *buf);
void init();
void mount_root();
int TST_bit(char buf[], int bit);
int SET_bit(char buf[], int bit);
int CLR_bit(char buf[ ], int bit);
void printdir(INODE ind);
int ialloc(int dev);
int idalloc(int dev, int ino);
int balloc(int dev);
int bdalloc(int dev, int ino);
int complete_tokenizer(char pathname[], char **child, MINODE **pip);
int my_mkdir(MINODE *pip, char *name);
int my_rmdir(MINODE *pip, char *name);
int my_mkfile(MINODE *pip, char *name);
int my_rmfile(MINODE *pip, char *name);
void new_Minode(int ino, int bnum, int fod);
void new_directory(MINODE *pip, int ino, int bnum, char buf[]);
int checkdir(MINODE *mip);
int decFreeInodes(int dev);
MINODE *helperfunctionalpha(char *pather);
int linkit(MINODE *first, MINODE *second, char *orig,  char *name);
void menu();
int make_dir(char pathname[], char *parmeter);  
void cd(char pathname[], char *parameter);   
void pwd(cwd);
void ls(char pathname[], char *parameter);   
void cat_file(char pathname[]);   
void stater(char pathname[]);
int remove_dir(char pathname[], char *parameter);
int creat_file(char pathname[], char *parameter);
int rm_file(char pathname[], char *parameter);
void linker(char pathname[], char *parameter);
void ulink(char pathname[], char *parameter);
void sylink(char pathname[], char *parameter);
void quit();
void my_truncate(MINODE *mip);
void chmod_file(char pathname[], char *parameter);
void chown_file(char pathname[], char *parameter);
void touch_file(char pathname[], char *parameter);
void open_file(char pathname[], char *parameter);
void close_file(char pathname[], char *parameter);
void lseek_file(char pathname[], char *parameter);
void read_file(char pathname[], char *parameter);
void write_file(char pathname[], char *parameter);
void cp_file(char pathname[], char *parameter);
void mv_file(char pathname[], char *parameter);
void mount(char pathname[], char *parameter);
void umount(char pathname[], char *parameter);
void wrongcmd();