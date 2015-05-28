#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void findHomeDirectory(char *env[]);
void commandProcessor(char *env[]);
void processPipe(char *pipe, char *env[]);

#endif
