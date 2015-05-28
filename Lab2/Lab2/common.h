/**********************************************************************************************
* Programmer: Christian Webber
* Group #1, Seq #19
* Class: CptS 360, Spring 2014;
* Lab 2
* Created: February 4th, 2014
* Last Revised: February 4th, 2014
* File: common.h
*********************************************************************************************/

#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////// Global Variables ////////////
#define BASE 10
#define HEX 16
char *table = "0123456789ABCDEF";

//////////// Function Definitions ////////////
// Main Functions
void part1_main(void);
void part2_main(void);
// Part 1 Functions
int myprintf(char *fmt, ...);
void printd(int x);
void printx(unsigned long x);
void printc(char c);
void prints(char *s);
void printu(unsigned long x);
void rpd(int x);
void rpx(unsigned long x);
void rpu(unsigned long x);
// Part 2 Functions
int A(void);
int B(void);
int C(void);
int longjump(int v);
int save(void);

#endif
