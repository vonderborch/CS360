#ifndef UTIL_H
#define UTIL_H

#include "type.h"

void get_block(int fd,int block, char *buf);
void put_block(int fd, int block, char *buf);
int token_path(char *pathname);
char *dirname(char *pathname);
char *basename(char *pathname);
unsigned long  getino(int *dev, char *pathname);
unsigned long  search(MINODE *mip, char *filename);
MINODE *iget(int dev, unsigned long ino);
void iput(MINODE *mip);
int findmyname(MINODE *parent, unsigned long myino, char *myname);
int findino(MINODE *mip, unsigned long *myino, unsigned long *parentino);
unsigned long ialloc(int dev);
unsigned long idealloc(int dev, unsigned long ino);
unsigned long balloc(int dev);
unsigned long bdealloc(int dev, unsigned long iblock);
void incFreeInodes(int dev);
void decFreeInodes(int dev);
void incFreeBlocks(int dev);
void decFreeBlocks(int dev);
int tst_bit(char *buf, int BIT);
int set_bit(char *buf, int BIT);
int clear_bit(char *buf, int BIT);
void patherror(char *cmdtemp);

#endif
