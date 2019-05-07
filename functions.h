#define COMMANDSIZE 1 //largura de um comando como os do programa ma, "i", "n", "p"...
#define NAMESIZE 80 //largura de um nome de artigo
#define IDSIZE 4 //largura de um id de artigo
#define STRINGIDSIZE 4 //largura de um id de uma string referente ao nome
#define PRICESIZE 5 //largura de um preco de artigo
#define ARTIGOSIZE (IDSIZE+STRINGIDSIZE+PRICESIZE+3) //dois ' ' e um '\n', largura de um artigo

#define FIFOSIZE 10 //largura do fifo de um cliente
#define QUANTSIZE 4 //largura de uma quantidade como as do programa cv
#define ARGSIZE (IDSIZE+QUANTSIZE+2)//um ' ' e um '\n', largura de um argumento do programa cv

void replaceFileContent(int fd, int offset, int size, char* artigo);
char* fillZeros(int number, int size);
int countNewLines(int fd);
void clearMem(char* buffer);
int findWordLine(int fd, char* word);
int readUntil(int fd, char c1, char c2, char* text);
char* initString(int size);
int openStrings(int flags);
int openArtigos(int flags);