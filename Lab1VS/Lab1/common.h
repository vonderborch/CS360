/**********************************************************************************************
 * Programmer: Christian Webber
 * Group #1, Seq #19
 * Class: CptS 360, Spring 2014;
 * Lab 1
 * Created: January 29th, 2014
 * Last Revised: January 30th, 2014
 * File: common.h
 *********************************************************************************************/

#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	char name[64];
	char type;
	struct node *childPtr, *siblingPtr, *parentPtr;
} NODE;

int initialize();
void reint();
void invalid();
void help();
void mkdir();
void rmdir();
void cd();
void ls();
void pwd();
void creat();
void rm();
void save();
void reload();
void saver(FILE *output, NODE *curnode);
void clearer(NODE **curnode);
void removeNode(char type);
void addNode(char type);
int findCommand(char command[64]);
int isAbsolute(char path[64]);
void updateCWDString();
int hasPathName(char pathname[64]);
int getDirNode();
void breakupPathName();

#endif
