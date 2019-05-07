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
	int fd = open("./ma.txt", O_RDONLY|O_APPEND, 0666);//abre ma.txt para ler e prepara strings de leitura
	if(fd<=0) {printf("Error opening ma.txt file, there should be an ma.txt to process\n"); return -1;}
	char* command = initString(COMMANDSIZE);
	char* name = initString(NAMESIZE);
	char* price = initString(PRICESIZE);
	char* artigo = initString(ARTIGOSIZE);
	char* id = initString(IDSIZE);

	int loop = 1;
	while(loop)//lê linha do ficheiro ma.txt
	{
		readUntil(fd, ' ', '\n', command);//lê o commando que vai ser usado na linha

		if(!strcmp(command, "i"))
		{
			if(readMaLine(fd, name, price)<=0) loop = 0;

			int fdStrings = openStrings(O_RDONLY|O_APPEND|O_CREAT); if(fdStrings<0) return fdStrings;
			int idString = findWordLine(fdStrings, name);//determina id do nome

			int fdArtigos = openArtigos(O_RDWR|O_APPEND|O_CREAT); if(fdArtigos<0) return fdArtigos;
			int newId = countNewLines(fdArtigos);//determina id do artigo

			concatArtigosLine(newId, idString, atoi(price), artigo);//cria string do artigo
			write(fdArtigos, artigo, ARTIGOSIZE);//escreve artigo em ARTIGOS.txt
		}
		else if(!strcmp(command, "n"))
		{
			if(readMaLine(fd, id, name)<=0) loop = 0;

			int fdStrings = openStrings(O_RDONLY|O_APPEND|O_CREAT); if(fdStrings<0) return fdStrings;
			int idString = findWordLine(fdStrings, name);//determina id do nome

			int fdArtigos = openArtigos(O_RDWR|O_CREAT); if(fdArtigos<0) return fdArtigos;

			concatArtigosLine(atoi(id), idString, 0, artigo);//cria string do artigo
			replaceFileContent(fdArtigos, atoi(id)*ARTIGOSIZE+IDSIZE+1, STRINGIDSIZE, artigo+IDSIZE+1);//escreve em ARTIGOS.txt o id do novo nome
		}
		else if(!strcmp(command, "p"))
		{
			if(readMaLine(fd, id, price)<=0) loop = 0;

			int fdArtigos = openArtigos(O_RDWR|O_CREAT); if(fdArtigos<0) return fdArtigos;

			concatArtigosLine(atoi(id), 0, atoi(price), artigo);//cria string do artigo
			replaceFileContent(fdArtigos, atoi(id)*ARTIGOSIZE+IDSIZE+STRINGIDSIZE+2, PRICESIZE, artigo+IDSIZE+STRINGIDSIZE+2);//escreve em ARTIGOS.txt o novo preco
		}

		clearMem(command);
		clearMem(name);
		clearMem(price);
		clearMem(artigo);
		clearMem(id);
	}

	free(command);
	free(name);
	free(price);
	free(artigo);
	free(id);
	return 0;
}