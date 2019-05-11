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

void getFileContent(int fd, int offset, int size, char* text)//substitui o conteúdo no offset pretendido pelo conteúdo de text até ao size, offset é onde começa a substituição, size é o tamanho da string por substituir e text é a string em si
{
	lseek(fd, offset, SEEK_SET);
	read(fd, text, size);
}

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
	int counter = -1;
	int readResult = -1;
	char* found = initString(NAMESIZE);

	do
	{
		clearMem(found);
		readResult = readUntil(fd, ' ', '\n', found);
		counter++;
	}
	while(strcmp(word, found) && readResult>0);

	free(found);
	if(readResult<=0) return -1; else return counter;
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
	for(int i=0; i<=size; i++) buffer[i]='\0';

	return buffer;
}

int openVendas(int flags)//abre o ficheiro VENDAS
{
	int fd = open("./files/VENDAS", flags, 0666);
	if(fd<=0) printf("Error opening VENDAS file\n");
	return fd;
}

int openStocks(int flags)//abre o ficheiro STOCKS
{
	int fd = open("./files/STOCKS", flags, 0666);
	if(fd<=0) printf("Error opening STOCKS file\n");
	return fd;
}

int openStrings(int flags)//abre o ficheiro STRINGS
{
	int fd = open("./files/STRINGS", flags, 0666);
	if(fd<=0) printf("Error opening STRINGS file\n");
	return fd;
}

int openArtigos(int flags)//abre o ficheiro ARTIGOS
{
	int fd = open("./files/ARTIGOS", flags, 0666);
	if(fd<=0) printf("Error opening ARTIGOS file\n");
	return fd;
}

int initConnect(char* fifo)//inicializa uma conexão fifo
{
	mkfifo(fifo, 0666);
	int fd = open(fifo, O_RDWR, 0666); if(fd<0) {printf("Error opening fifo\n"); return -1;}

	return fd;
}

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

char* getClientFifo(int pid)//obtém o fifo do cliente dado um inteiro que representa o seu pid
{
	char* fifo = initString(FIFOSIZE);
	sprintf(fifo, "./fifo/fifo%d", pid);

	return fifo;
}

void addNewLine(char* text)//adiciona uma mudança de linha no fim de uma string
{
	char* buffer = strdup("1");
	buffer[0]='\n';
	text = strcat(text, buffer);

	free(buffer);
}

int getStock(char* id)//obtém o stock do id respectivo
{
	int fd = openStocks(O_RDONLY); if(fd<=0) return -1;
	char* stock = initString(QUANTSIZE);

	getFileContent(fd, atoi(id)*STOCKSIZE, QUANTSIZE, stock);
	close(fd);
	int stockInt = atoi(stock);

	free(stock);
	return stockInt;
}

int getPrice(char* id)//obtém o preço do id respectivo
{
	int fd = openArtigos(O_RDONLY); if(fd<=0) return -1;
	char* price = initString(PRICESIZE);

	getFileContent(fd, atoi(id)*ARTIGOSIZE+IDSIZE+STRINGIDSIZE+2, PRICESIZE, price);
	close(fd);
	int priceInt = atoi(price);

	free(price);
	return priceInt;
}

void concatVendasLine(int id, int price, int quant, char* venda)//pega em id, price e quant e junta numa string venda com formato "id price quant\n"
{
	char* idS = fillZeros(id, IDSIZE);
	char* priceS = fillZeros(price, PRICESIZE);
	char* quantS = fillZeros(quant, QUANTSIZE);

	char* space = strdup(" ");
	char* newLine = strdup("\n");
	venda = strcat(venda, idS);
	venda = strcat(venda, space);
	venda = strcat(venda, priceS);
	venda = strcat(venda, space);
	venda = strcat(venda, quantS);
	venda = strcat(venda, newLine);

	free(idS);
	free(priceS);
	free(quantS);
	free(space);
	free(newLine);
}

int atualizaStock(char* id, char* quant)//atualiza o ficheiro STOCKS dependendo da quantidade, se for positiva adiciona stock, se for negativa retira stock e também adiciona uma linha ao ficheiro VENDAS
{
	int stock = getStock(id);
	stock = stock + atoi(quant); if(stock<0) return -1;

	char* stockS = fillZeros(stock, QUANTSIZE);

	int fdStocks = openStocks(O_WRONLY); if(fdStocks<=0) return -1;
	replaceFileContent(fdStocks, atoi(id)*STOCKSIZE, QUANTSIZE, stockS);
	close(fdStocks);

	if(atoi(quant)<0)
	{
		int price = getPrice(id);
		price = price * (-atoi(quant));

		char* venda = initString(VENDASIZE);
		concatVendasLine(atoi(id), price, -atoi(quant), venda);

		int fdVendas = openVendas(O_WRONLY|O_APPEND|O_CREAT); if(fdVendas<=0) return -1;
		write(fdVendas, venda, strlen(venda));
		close(fdVendas);

		free(venda);
	}

	free(stockS);
	return stock;
}