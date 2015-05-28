#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "type.h"

//////////////// GLOBALS ////////////////
extern functionTable ftable[];

//////////////// OTHER FUNCTIONS ////////////////
void startuphelp();
void init(char *tempcmd);
void mount_root(char *tempcmd);
void find_and_execute_command(char *tempcmd);
void savecommand(char tempcmd[256]);

//////////////// COMMAND FUNCTIONS ////////////////
int CMD_LAST();
int CMD_TOGGLESLEEP();
int CMD_CLEAR();
int CMD_MENU();
int CMD_REINT();
void CMD_QUIT();
int CMD_CD();
int CMD_LS();
int CMD_MKDIR();
int CMD_CREAT();
int CMD_RMDIR();
int CMD_RM();
int CMD_LINK();
int CMD_UNLINK();
int CMD_SYMLINK();
int CMD_TOUCH();
int CMD_CHMOD();
int CMD_CHOWN();
int CMD_CHGRP();
int CMD_STAT();
int CMD_PWD();

//////////////// HELPER FUNCTIONS ////////////////
int do_ls(char *path);
void printFile(MINODE *mip, char *namebuf);
void printChild(int devicename, MINODE *mp);
int findparent(char *pathn);
int my_mkdir(MINODE *pip, char *name);
int my_creat(MINODE *pip, char *name);
int isEmpty(MINODE *mip);
int rm_child(MINODE *parent, char *my_name);
void do_pwd(MINODE *wd);
int do_stat(char *path, struct stat *stPtr);
int do_unlink(int forcerm);

#endif
