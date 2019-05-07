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

void replaceFileContent(int fd, int offset, int size, char* text)//substitui o conteúdo no offset pretendido pelo conteúdo de text até ao size, offset é onde começa a substituição, size é o tamanho da string por substituir e text é a string em si
{
	lseek(fd, offset, SEEK_SET);
	write(fd, text, size);
}

char* fillZeros(int number, int size)//adiciona zeros atras de um numero até chegar a um size e retorna a string formada
{
	char* numberS = initString(size);
	char* tempNumberS = initString(size);

	sprintf(numberS, "%d", number);
	strcpy(tempNumberS, numberS);
	while(strlen(numberS)<size)
	{
		sprintf(tempNumberS, "0%s", numberS);
		strcpy(numberS, tempNumberS);
	}

	free(tempNumberS);
	return numberS;
}

int countNewLines(int fd)//procura quantas mudanças de linha existem no ficheiro, serve para conhecer o id do artigo a inserir
{
	int counter=0;

	char* buffer = strdup("1");
	while(read(fd, buffer, 1)>0) if(buffer[0]=='\n') counter++;

	return counter;
}

void clearMem(char* buffer)//limpa o buffer
{
	memset(buffer,0,strlen(buffer));
}

int findWordLine(int fd, char* word)//procura a word no ficheiro e retorna o número da linha em que se encontra
{
	int counter=-1;
	char* found = initString(NAMESIZE);

	do
	{
		clearMem(found);
		readUntil(fd, '\n', '\n', found);
		counter++;
	}
	while(strcmp(word, found));

	free(found);
	return counter;
}

int readUntil(int fd, char c1, char c2, char* text)//lê o ficheiro até encontrar um dos caracteres escolhidos, coloca o que encontrar em text
{
	int readResult;

	char* buffer = strdup("1");
	while((readResult = read(fd, buffer, 1))>0 && buffer[0]!=c1 && buffer[0]!=c2) text = strcat(text, buffer);

	free(buffer);
	return readResult;
}

char* initString(int size)//inicializa uma string com nada dentro
{
	char* buffer = malloc(sizeof(char)*size+1);
	buffer[0]='\0';

	return buffer;
}

int openStrings(int flags)//abre o ficheiro STRINGS
{
	int fd = open("./STRINGS.txt", flags, 0666);
	if(fd<=0) printf("Error opening STRINGS.txt file\n");
	return fd;
}

int openArtigos(int flags)//abre o ficheiro ARTIGOS
{
	int fd = open("./ARTIGOS.txt", flags, 0666);
	if(fd<=0) printf("Error opening ARTIGOS.txt file\n");
	return fd;
}