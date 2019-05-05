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

#define COMMANDSIZE 1
#define NAMESIZE 80
#define IDSIZE 4
#define STRINGIDSIZE 4
#define PRICESIZE 5
#define ARTIGOSIZE (IDSIZE+STRINGIDSIZE+PRICESIZE+3)

void replaceFileContent(int fd, int offset, int size, char* artigo)//substitui o conteúdo no offset pretendido pelo conteúdo de artigo até ao size
{
	lseek(fd, offset, SEEK_SET);
	write(fd, artigo, size);
}

void concatArtigosLine(int id, int idString, int price, char* artigo)//pega em id, idString e price e junta numa string artigo com formato "id idString price\n"
{
	char* zero = strdup("0");
	char* space = strdup(" ");
	char* newLine = strdup("\n");

	char idS[IDSIZE];
	sprintf(idS, "%d", id);
	while(strlen(idS)<IDSIZE) {strcat(zero, idS); sprintf(idS, "%s", zero); zero = strdup("0");}

	char idStringS[STRINGIDSIZE];
	sprintf(idStringS, "%d", idString);
	while(strlen(idStringS)<STRINGIDSIZE) {strcat(zero, idStringS); sprintf(idStringS, "%s", zero); zero = strdup("0");}

	char priceS[PRICESIZE];
	sprintf(priceS, "%d", price);
	while(strlen(priceS)<PRICESIZE) {strcat(zero, priceS); sprintf(priceS, "%s", zero); zero = strdup("0");}
	
	artigo = strcat(artigo, idS);
	artigo = strcat(artigo, space);
	artigo = strcat(artigo, idStringS);
	artigo = strcat(artigo, space);
	artigo = strcat(artigo, priceS);
	artigo = strcat(artigo, newLine);
}

int countNewLines(int fd)//procura quantas mudanças de linha existem no ficheiro, serve para conhecer o id do artigo a inserir
{
	int counter=0;
	char buffer[1];

	while(read(fd, buffer, 1)>0) if(buffer[0]=='\n') counter++;

	return counter;
}

void clearMem(char* buffer)//limpa o buffer, os free não estavam a apagar tudo
{
	memset(buffer,0,strlen(buffer));
}

int readToSpace(int fd, char* word)//lê o ficheiro até encontrar um espaço ou uma mudança de linha, coloca o que encontrar em word
{
	int readResult;
	char buffer[1];

	while((readResult = read(fd, buffer, 1))>0 && buffer[0]!=' ' && buffer[0]!='\n')
	{
		word = strcat(word, buffer);
	}

	return readResult;
}

int findWordLine(int fd, char* word)//procura a word no ficheiro e retorna o número da linha em que se encontra
{
	int counter=-1;
	char* found = malloc(sizeof(char)*NAMESIZE+1);

	do
	{
		clearMem(found);
		readToSpace(fd, found);
		counter++;
	}
	while(strcmp(word, found));

	free(found);
	return counter;
}

int readMaLine(int fd, char* fst, char* snd)//lê uma linha do formato do ficheiro ma.txt e guarda as respectivas variavéis com o que encontra
{
	readToSpace(fd, fst);
	int readResult = readToSpace(fd, snd);

	return readResult;
}

int main(int argc, char* argv[])
{
	int fd = open("./ma.txt", O_RDONLY|O_APPEND, 0666);//abre ma.txt para ler e prepara strings de leitura
	if(fd<=0) {printf("Error opening ma.txt file, there should be an ma.txt to process\n"); return -1;}
	char* command = malloc(sizeof(char)*COMMANDSIZE+1);
	char* name = malloc(sizeof(char)*NAMESIZE+1);
	char* price = malloc(sizeof(char)*PRICESIZE+1);
	char* artigo = malloc(sizeof(char)*ARTIGOSIZE+1);
	char* id = malloc(sizeof(char)*IDSIZE+1);

	int loop = 1;
	while(loop)//lê linha do ficheiro ma.txt
	{
		readToSpace(fd, command);//lê o commando que vai ser usado na linha

		if(!strcmp(command, "i"))
		{
			if(readMaLine(fd, name, price)<=0) loop = 0;

			int fdStrings = open("./STRINGS.txt", O_RDONLY|O_APPEND|O_CREAT, 0666);//abre STRINGS.txt para ler e transforma nome encontrado no id respetivo
			if(fd<=0) {printf("Error opening STRINGS.txt file\n"); return -1;}
			int idString = findWordLine(fdStrings, name);

			int fdArtigos = open("./ARTIGOS.txt", O_RDWR|O_APPEND|O_CREAT, 0666);//abre ARTIGOS.txt para ler e escrever
			if(fd<=0) {printf("Error opening ARTIGOS.txt file\n"); return -1;}
			int newId = countNewLines(fdArtigos);

			concatArtigosLine(newId, idString, atoi(price), artigo);//cria string do artigo e escreve em ARTIGOS.txt
			write(fdArtigos, artigo, ARTIGOSIZE);
		}
		else if(!strcmp(command, "n"))
		{
			if(readMaLine(fd, id, name)<=0) loop = 0;

			int fdStrings = open("./STRINGS.txt", O_RDONLY|O_APPEND|O_CREAT, 0666);//abre STRINGS.txt para ler e transforma nome encontrado no id respetivo
			if(fd<=0) {printf("Error opening STRINGS.txt file\n"); return -1;}
			int idString = findWordLine(fdStrings, name);

			int fdArtigos = open("./ARTIGOS.txt", O_RDWR|O_CREAT, 0666);//abre ARTIGOS.txt para ler e escrever
			if(fd<=0) {printf("Error opening ARTIGOS.txt file\n"); return -1;}

			concatArtigosLine(atoi(id), idString, 0, artigo);//cria string do artigo e escreve em ARTIGOS.txt o id do novo nome
			replaceFileContent(fdArtigos, atoi(id)*ARTIGOSIZE+IDSIZE+1, STRINGIDSIZE, artigo+IDSIZE+1);
		}
		else if(!strcmp(command, "p"))
		{
			if(readMaLine(fd, id, price)<=0) loop = 0;

			int fdArtigos = open("./ARTIGOS.txt", O_RDWR|O_CREAT, 0666);//abre ARTIGOS.txt para ler e escrever
			if(fd<=0) {printf("Error opening ARTIGOS.txt file\n"); return -1;}

			concatArtigosLine(atoi(id), 0, atoi(price), artigo);//cria string do artigo e escreve em ARTIGOS.txt o novo preco
			replaceFileContent(fdArtigos, atoi(id)*ARTIGOSIZE+IDSIZE+STRINGIDSIZE+2, PRICESIZE, artigo+IDSIZE+STRINGIDSIZE+2);
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