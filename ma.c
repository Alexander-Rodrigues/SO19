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

void concatArtigosLine(int id, int idString, int price, char* artigo)//pega em id, idString e price e junta numa string artigo com formato "id idString price\n"
{
	char* idS = fillZeros(id, IDSIZE);
	char* idStringS = fillZeros(idString, STRINGIDSIZE);
	char* priceS = fillZeros(price, PRICESIZE);

	char* space = strdup(" ");
	char* newLine = strdup("\n");
	artigo = strcat(artigo, idS);
	artigo = strcat(artigo, space);
	artigo = strcat(artigo, idStringS);
	artigo = strcat(artigo, space);
	artigo = strcat(artigo, priceS);
	artigo = strcat(artigo, newLine);

	free(idS);
	free(idStringS);
	free(priceS);
	free(space);
	free(newLine);
}

int readMaLine(int fd, char* fst, char* snd)//lê uma linha do formato do ficheiro ma.txt e guarda as respectivas variavéis com o que encontra
{
	readUntil(fd, ' ', '\n', fst);
	int readResult = readUntil(fd, ' ', '\n', snd);//se o segundo for end of file fica-se com o resultado

	return readResult;
}

int main(int argc, char* argv[])
{
	int loop = 1;
	while(loop)//lê linhas do stdin
	{
		char* command = initString(COMMANDSIZE);
		char* name = initString(NAMESIZE);
		char* price = initString(PRICESIZE);
		char* artigo = initString(ARTIGOSIZE);
		char* id = initString(IDSIZE);

		readUntil(0, ' ', '\n', command);//lê o commando que vai ser usado na linha
		if(!strcmp(command, "i"))
		{
			if(readMaLine(0, name, price)<=0) loop = 0;

			int idString = -1;
			while(idString<0)
			{
				int fdStrings = openStrings(O_RDWR|O_APPEND|O_CREAT); if(fdStrings<0) return fdStrings;
				idString = findWordLine(fdStrings, name);//determina id do nome
				if(idString<0) {write(fdStrings, name, strlen(name)); write(fdStrings, "\n", 1);}//escreve novo nome caso não seja encontrado
				close(fdStrings);
			}

			int fdArtigos = openArtigos(O_RDWR|O_APPEND|O_CREAT); if(fdArtigos<0) return fdArtigos;
			int newId = countNewLines(fdArtigos);//determina id do artigo

			concatArtigosLine(newId, idString, atoi(price), artigo);//cria string do artigo
			write(fdArtigos, artigo, ARTIGOSIZE);//escreve o novo artigo em ARTIGOS
			close(fdArtigos);

			int fdStocks = openStocks(O_WRONLY|O_APPEND|O_CREAT); if(fdStocks<0) return fdStocks;

			char* stockS = fillZeros(0, QUANTSIZE);
			addNewLine(stockS);
			write(fdStocks, stockS, STOCKSIZE);//adiciona o novo artigo ao STOCKS com quantidade 0
			close(fdStocks);

			free(stockS);
		}
		else if(!strcmp(command, "n"))
		{
			if(readMaLine(0, id, name)<=0) loop = 0;

			int idString = -1;
			while(idString<0)
			{
				int fdStrings = openStrings(O_RDWR|O_APPEND|O_CREAT); if(fdStrings<0) return fdStrings;
				idString = findWordLine(fdStrings, name);//determina id do nome
				if(idString<0) {write(fdStrings, name, strlen(name)); write(fdStrings, "\n", 1);}//escreve novo nome caso não seja encontrado
				close(fdStrings);
			}

			int fdArtigos = openArtigos(O_RDWR|O_CREAT); if(fdArtigos<0) return fdArtigos;

			concatArtigosLine(atoi(id), idString, 0, artigo);//cria string do artigo
			replaceFileContent(fdArtigos, atoi(id)*ARTIGOSIZE+IDSIZE+1, STRINGIDSIZE, artigo+IDSIZE+1);//escreve em ARTIGOS o id do novo nome
			close(fdArtigos);
		}
		else if(!strcmp(command, "p"))
		{
			if(readMaLine(0, id, price)<=0) loop = 0;

			int fdArtigos = openArtigos(O_RDWR|O_CREAT); if(fdArtigos<0) return fdArtigos;

			concatArtigosLine(atoi(id), 0, atoi(price), artigo);//cria string do artigo
			replaceFileContent(fdArtigos, atoi(id)*ARTIGOSIZE+IDSIZE+STRINGIDSIZE+2, PRICESIZE, artigo+IDSIZE+STRINGIDSIZE+2);//escreve em ARTIGOS o novo preco
			close(fdArtigos);
		}
		else if(!strcmp(command, "a"))
		{
			int fd = initConnect("./fifo/fifo"); if(fd<0) {return fd;}//conectar ao fifo do servidor
			int fdMa = initConnect("./fifo/fifoma"); if(fdMa<0) {return fdMa;}//conectar ao fifo do ma

			write(fd, "./fifo/fifoma ag\n", strlen("./fifo/fifoma ag\n"));
			close(fd);

			char* res = initString(RESSIZE);//inicializar a string de resposta
			readUntil(fdMa, 'v', 'v', res);//ler resposta do servidor, a resposta acaba com um caracter 'v'
			close(fdMa);
			printf("%s\n", res);//imprimir resposta

			free(res);
		}

		free(command);
		free(name);
		free(price);
		free(artigo);
		free(id);
	}

	return 0;
}