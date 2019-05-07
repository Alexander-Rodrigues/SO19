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

int main(int argc, char* argv[])
{
	char* fifo = strdup("./fifo");
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_WRONLY, 0666); if(fd<0) {printf("Error opening fifo\n"); return -1;}

	while(1)
	{
		char* arg = initString(ARGSIZE);
		readUntil(0, '\n', '\n', arg);

		char* buffer = strdup("1");
		buffer[0]='\n';
		arg = strcat(arg, buffer);
		
		write(fd, arg, strlen(arg));

		free(buffer);
		free(arg);
	}
}