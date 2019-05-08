#define COMMANDSIZE 1 //largura de um comando como os do programa ma, "i", "n", "p"...
#define IDSIZE 4 //largura de um id de artigo
#define NAMESIZE 80 //largura de um nome de artigo
#define STRINGIDSIZE 4 //largura de um id de uma string referente ao nome
#define PRICESIZE 5 //largura de um preco de artigo
#define QUANTSIZE 4 //largura de uma quantidade como as do programa cv
#define ARTIGOSIZE (IDSIZE+STRINGIDSIZE+PRICESIZE+3) //dois ' ' e um '\n', largura de um artigo
#define VENDASIZE (IDSIZE+PRICESIZE+QUANTSIZE+3) //dois ' ' e um '\n', largura de uma venda
#define STOCKSIZE (QUANTSIZE+1) //um ' ' e um '\n', largura de um stock
#define PIDSIZE 4 //largura de uma quantidade como as do programa cv
#define FIFOSIZE (PIDSIZE+9) //largura do fifo de um cliente, inclui "./fifo" no inicio
#define ARGSIZE (FIFOSIZE+IDSIZE+QUANTSIZE+3)//dois ' ' e um '\n', largura de um argumento do programa cv


void replaceFileContent(int fd, int offset, int size, char* artigo);
char* fillZeros(int number, int size);
int countNewLines(int fd);
void clearMem(char* buffer);
int findWordLine(int fd, char* word);
int readUntil(int fd, char c1, char c2, char* text);
char* initString(int size);
int openSTOCKS(int flags);
int openStrings(int flags);
int openArtigos(int flags);
int initConnect();
int splitArgs(char* arg, char** args);
char* getClientFifo(int pid);
void addNewLine(char* text);
int getStock(char* id);
int getPrice(char* id);
void concatVendasLine(int id, int price, int quant, char* venda);
int atualizaStock(char* id, char* quant);