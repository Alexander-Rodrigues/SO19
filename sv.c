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
	int fd = initConnect(strdup("./fifo/fifo")); if(fd<0) {return -1;}//conectar ao fifo do servidor
	printf("Server running...\n");

	while(1)
	{
		char* arg = initString(ARGSIZE);//inicializar a string de argumentos
		readUntil(fd, '\n', '\n', arg);

		char** args = malloc(sizeof(char*)*2);//separar a string nos diferentes argumentos
		int nArgs = splitArgs(arg, args);

		char* res = initString(QUANTSIZE+PRICESIZE+20);//inicializar a string de resposta
		if(nArgs==2)
		{
			int stock = getStock(args[1]);//obtém o stock do id respectivo
			int preco = getPrice(args[1]);//obtém o preço do id respectivo
			if(stock>=0 && preco>=0) sprintf(res, "Stock: %d, Price: %d\n", stock, preco);//guarda resultado em res
			else sprintf(res, "Error, try again!\n");
		}
		else if(nArgs==3)
		{
			int stock = atualizaStock(args[1], args[2]);//atualiza o stock e adiciona uma venda
			if(stock>=0) sprintf(res, "New Stock: %d\n", stock);//guarda resultado em res
			else sprintf(res, "Error, try again!\n");
		}
		else
		{
			sprintf(res, "Error, try again!\n");
		}

		int fdClient = initConnect(args[0]);//conectar ao fifo do cliente, o primeiro argumento é o nome do fifo
		if(fdClient>0) write(fdClient, res, strlen(res));//enviar a resposta ao cliente
		close(fdClient);

		free(res);
		free(arg);
		free(args);
	}
	
	return 0;
}