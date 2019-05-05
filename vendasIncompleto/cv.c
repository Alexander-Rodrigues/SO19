#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h> //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <sys/wait.h> //chamadas wait*() e macros relacionadas
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
typedef void (*sighandler_t)(int);

#define FIFOSIZE 10

int connect(char* fifo)
{
	int fd = open(fifo, O_RDWR, 0666);
	return fd;
}

int initConnect(char* nextFifo)
{
	char fifo[7];
	sprintf(fifo, "./fifo");
	int fd = connect(fifo); if(fd<0) return fd;

	char buffer[2];
	sprintf(buffer, "1");
	write(fd, buffer, 1);

	read(fd, nextFifo, FIFOSIZE);
	close(fd);

	return 1;
}

int main(int argc, char* argv[])
{
	char fifo[FIFOSIZE];
	if(initConnect(fifo)<0) return -1;

	int fd = connect(fifo); if(fd<0) return fd;

	int buffer[2];
	read(0, buffer, 1);
	write(fd, buffer, 1);
}