#include <fcntl.h> //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h> //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <sys/wait.h> //chamadas wait*() e macros relacionadas
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>
#include <float.h>
#include "functions.h"
typedef void(*sighandler_t)(int);

//Attemps to find the Line in which id is, if no match is found returns the line in where the new value.
int findLn(int fd, int id, int price, int qtd, char* buff){
	lseek(fd,0,SEEK_SET);
	int i = 0;
	int r;
	int size = AGRSIZE;
	while (1) {
	r = read(fd,buff,AGRSIZE);	
		
		if (r < AGRSIZE - 1) {
			break;
		}
		
		if (atoi(buff) == id) {
			price += atoi(buff + IDSIZE + 1);
			qtd += atoi(buff + IDSIZE + PRICESIZE + 2);
			break;
		}
		i++;
		
	}
	lseek(fd,i * size,SEEK_SET);
	sprintf(buff,"%0*d %0*d %0*d\n",IDSIZE, id, PRICESIZE, price, QUANTSIZE, qtd);
	write(fd,buff,AGRSIZE);
	return i;
}

//Tries to find the value in the cache, if it hits, reads and writes directly.
//On miss, calls findLn and places the new value on the lowest hit cache spot.
//In a perfect world I guess it'd never miss huh?
int tryCache(int fd, int id, int price, int qtd, char* buff, int cache[CACHESIZE][3]) {
	int i = 0;
	int pos = -1;
	int leastHits = cache[0][2];
	int leastHitPos = -1;
	int size = AGRSIZE;
	while (i < CACHESIZE) {
		//cache hit
		if (cache[i][0] == id) {
			cache[i][2] += 1;
			pos = cache[i][1];
			
			lseek(fd,pos * size,SEEK_SET);
			read(fd,buff,AGRSIZE);
			price += atoi(buff + IDSIZE + 1);
			qtd += atoi(buff + IDSIZE + PRICESIZE + 2);
			lseek(fd,pos * size,SEEK_SET);
			sprintf(buff,"%0*d %0*d %0*d\n",IDSIZE, id, PRICESIZE, price, QUANTSIZE, qtd);
			write(fd,buff,AGRSIZE);

			break;
		}
		
		if (cache[i][2] <= leastHits) {
			leastHits = cache[i][2];
			leastHitPos = i;
		}
		i++;
	}
	//miss or fresh
	if (pos < 0) {
		cache[leastHitPos][0] = id;
		cache[leastHitPos][1] = findLn(fd, id, price, qtd, buff);
		pos = cache[leastHitPos][1];
		cache[leastHitPos][2] = 0;
	}
	return pos;
}

//Creates a buffer to read each line from STDIN, opens a file with name PIDSIZE.
//Creates a CACHESIZEx3 matrix and fills it with -1;(Could be filled with 0s and calloc'ed but prefer it on the stack
//Reads each line from the STDIN to the buffer, breaks when a line is smaller then it should be
//Before breaking each value is put on a variable(id, price, qtd).
//For each line tryCache is called with those 3 values and the buffer;
//Prints the content from the temp file to stdOut;
//Note: the same buffer keeps getting used for efficiency.
int main(int argc, char* argv[]) {
	char* buff = malloc(sizeof(char) * (AGRSIZE));
	char tmpName[16]; sprintf(tmpName,"%d",getpid());
	//change this
	int tmp = open(tmpName, O_CREAT | O_RDWR, 0666);
	int r = AGRSIZE;

	int cache[CACHESIZE][3];
	int i;
	for (i = 0; i < CACHESIZE; i++) {
		cache[i][0] = -1;
		cache[i][1] = -1;
		cache[i][2] = -1;
	}

	while (1) {
		r = read(0,buff,AGRSIZE);
		if (r < AGRSIZE) break;
		int id = atoi(buff);
		int price = atoi(buff + IDSIZE + 1);
		int qtd = atoi(buff + IDSIZE + PRICESIZE + 2);
		tryCache(tmp, id,  price, qtd, buff, cache);
	}
	lseek(tmp,0L, SEEK_SET);
	while (1) {
		r = read(tmp,buff,AGRSIZE);
		if (r < AGRSIZE) break;
		printf("%s",buff);
	}
	free(buff);
	remove(tmpName);
}