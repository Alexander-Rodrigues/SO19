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
#define IDSIZE 4

int connect(char* fifo)
{
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDWR, 0666);

	return fd;
}

int initConnect(int counter)
{
	char fifo[7];
	sprintf(fifo, "./fifo");
	int fd = connect(fifo); if(fd<0) return fd;

	char buffer[2];
	read(fd, buffer, 1);

	char nextFifo[FIFOSIZE];
	sprintf(nextFifo, "./fifo%d", counter);
	write(fd, nextFifo, FIFOSIZE);

	close(fd);
	return 1;
}

int main(int argc, char* argv[])
{
	int counter = 0;
	while(1)
	{
		int pid;
		pid = fork();
		if(pid)//father
		{
			printf("Establishing connection number %d...\n", counter);
			if(initConnect(counter)>0) counter++;
		}
		else//child
		{
			char fifo[FIFOSIZE];
			sprintf(fifo, "./fifo%d", counter);
			int fd = connect(fifo); if(fd<0) return fd;

			char buffer[2];
			read(fd, buffer, 1);
			printf("Read %s\n", buffer);

			close(fd);
			return 0;
		}
	}
}