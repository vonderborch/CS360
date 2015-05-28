#include "type.h"

void get_block(int fd, int blk, char buf[BLOCK_SIZE])
{
	lseek(fd, (long)(blk * BLOCK_SIZE), 0);
	read(fd, buf, BLOCK_SIZE);
}

void put_block(int fd, int blk, char buf[BLOCK_SIZE])
{
	lseek(fd, (long)(blk * BLOCK_SIZE), 0);
	write(fd, buf, BLOCK_SIZE);
}

int tokenizer(char *string, char **outArray[64], char *delim)
{
	int n = 0;
	char *temp, *backup;
	backup = malloc(sizeof(char) * strlen(string));
	strcpy(backup, string);
	temp = strtok(backup, delim);
	while (temp != NULL)
	{
		outArray[n] = temp;
		outArray[n+1] = 0;
		n++;
		temp = strtok(0, delim);
	}
	if (n != 0)
		n--;
	return n;
}

char *dirname (char *string)
{
	int n, dirsize, basesize;
	char *output, *temparray[64];
	n = tokenizer(string, &temparray, "/");
	basesize = strlen(temparray[n]);
	dirsize = strlen(string) - basesize;
	output = malloc(sizeof(char) * dirsize);
	strncpy(output, string, dirsize);
	return output;
}

char *basename (char *string)
{
	int n, dirsize, basesize;
	char *output, *temparray[64];
	n = tokenizer(string, &temparray, "/");
	printf("%d: %s\n", n, temparray[n]);
	basesize = strlen(temparray[n]);
	output = malloc(sizeof(char) * basesize);
	output = temparray[n];
	return output;
}
