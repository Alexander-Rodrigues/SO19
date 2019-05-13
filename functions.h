#define COMMANDSIZE 1 //largura de um comando como os do programa ma, "i", "n", "p"...
#define IDSIZE 10 //largura de um id de artigo
#define PRICESIZE 10 //largura de um preco de artigo
#define QUANTSIZE 10 //largura de uma quantidade como as do programa cv
#define NAMESIZE 80 //largura de um nome de artigo
#define STRINGIDSIZE 10 //largura de um id de uma string referente ao nome
#define ARTIGOSIZE (IDSIZE+STRINGIDSIZE+PRICESIZE+3) //dois ' ' e um '\n', largura de um artigo
#define VENDASIZE (IDSIZE+PRICESIZE+QUANTSIZE+3) //dois ' ' e um '\n', largura de uma venda
#define STOCKSIZE (QUANTSIZE+1) //um ' ' e um '\n', largura de um stock
#define AGRSIZE (IDSIZE+1+PRICESIZE+1+QUANTSIZE+1) //largura de um agregador
#define RESSIZE (AGRSIZE*30) //largura de uma resposta do programa sv, pode conter resposta do programa ag
#define ARGSIZE (FIFOSIZE+IDSIZE+QUANTSIZE+3) //dois ' ' e um '\n', largura de um argumento do programa cv
#define PIDSIZE 10 //largura do pid de um cliente
#define FIFOSIZE (PIDSIZE+11) //largura do fifo de um cliente, inclui "./fifo/fifo" no inicio
#define CACHESIZE 5

void replaceFileContent(int fd, int offset, int size, char* artigo);
char* fillZeros(int number, int size);
int countNewLines(int fd);
void clearMem(char* buffer);
int findWordLine(int fd, char* word);
int readUntil(int fd, char c1, char c2, char* text);
char* initString(int size);
int openStocks(int flags);
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
int getOffset();
void setOffset(int offset);
int callAg(int* offset);