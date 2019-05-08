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
	int fd = initConnect(strdup("./fifo")); if(fd<0) {return -1;}//conectar ao fifo do servidor
	char* fifoClient = getClientFifo(getpid());//obter fifo do cliente através do seu pid
	int fdClient = initConnect(fifoClient); if(fdClient<0) {return -1;}//conectar ao fifo do cliente
	printf("Connected!\n");

	while(1)
	{
		char* arg = initString(ARGSIZE);//inicializar a string de argumentos
		sprintf(arg, "%s ", fifoClient);//colocar o fifo do cliente como primeiro argumento
		readUntil(0, '\n', '\n', arg);//ler resto dos argumentos
		addNewLine(arg);//finalizar argumentos com uma mudança de linha

		write(fd, arg, strlen(arg));//enviar a string de argumentos ao servidor

		char* resp = initString(FIFOSIZE);//inicializar a string de resposta
		readUntil(fdClient, '\n', '\n', resp);//ler resposta do servidor
		printf("%s\n", resp);//imprimir resposta

		free(resp);
		free(arg);
	}

	free(fifoClient);
}