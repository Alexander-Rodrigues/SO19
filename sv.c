#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h> //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <sys/wait.h> //chamadas wait*() e macros relacionadas
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "functions.h"
typedef void (*sighandler_t)(int);

int splitArgs(char* arg, char** args)//separa string fornecida em argumentos diferentes, separados por ' '
{
	int counter = 0;
	const char space[2] = " ";
	char* token = strtok(arg, space);

	while(token!=NULL)
	{
		args[counter] = token;
		token = strtok(NULL, space);
		counter++;
	}

	return counter;
}

int main(int argc, char* argv[])
{
	char* fifo = strdup("./fifo");
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDONLY, 0666); if(fd<0) {printf("Error opening fifo\n"); return -1;}

	while(1)
	{
		char* arg = initString(ARGSIZE);
		readUntil(fd, '\n', '\n', arg);

		char** args = malloc(sizeof(char*)*2);
		int nArgs = splitArgs(arg, args);

		for(int i=0;i<nArgs;i++) printf("Read %s\n", args[i]);

		free(arg);
		free(args);
	}
}