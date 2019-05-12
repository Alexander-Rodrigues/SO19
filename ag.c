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

void freeC(char** c){
	free(c[0]);
	free(c[1]);
	free(c[2]);
}

//Reads a line in the format, storing the values in the given params
int readLnC(int fd, char** inS, int* in) {	
	
	read(fd, inS[0], IDSIZE);
	lseek(fd, 1, SEEK_CUR);
	read(fd, inS[1], PRICESIZE);
	lseek(fd, 1, SEEK_CUR);
	int r = read(fd, inS[2], QUANTSIZE);
	lseek(fd, 1, SEEK_CUR);

	in[0] = atoi(inS[0]);
	in[1] = atoi(inS[1]);
	in[2] = atoi(inS[2]);

	return r;
}

//Returns the file position of the beggining of the line with id;
off_t getPos(int fd, char* id) {

	off_t end = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);
	off_t r = 0;
	while(r < end){
		lseek(fd,r,SEEK_SET);
		char* c = malloc(sizeof(char)* IDSIZE);
		read(fd,c,IDSIZE);
		if (strcmp(c,id) == 0) {
			
			return r;
			lseek(fd,0,SEEK_SET);
		}
		r += AGRSIZE;
	}
	return r;
	lseek(fd,0,SEEK_SET);
}

//Replaces the values at pos with the ones at in using s as a string buffer;
void replace(int fd, off_t pos, int* in, char** s) {
	
	lseek(fd,pos,SEEK_SET);
	int old[3];
	old[1] = 0;
	old[2] = 0;
	sprintf(s[1],"%0*d", PRICESIZE, 0);
	sprintf(s[2],"%0*d", QUANTSIZE, 0);
	readLnC(fd, s, old);
	
	in[1] += old[1];
	in[2] += old[2];
	
	sprintf(s[1],"%0*d", PRICESIZE, in[1]);
	sprintf(s[2],"%0*d", QUANTSIZE, in[2]);
	
	lseek(fd,pos,SEEK_SET);
	write(fd, s[0], IDSIZE);
	write(fd, " ", 1);
	write(fd, s[1], PRICESIZE);
	write(fd, " ", 1);
	write(fd, s[2], QUANTSIZE);
	write(fd, "\n", 1);
	
}

//Reads each line from stdin and for each one searches for its values in the tmp file inserting them;
int main(int argc, char* argv[]) {
	char* fileName = malloc(sizeof(char) * 8);
	sprintf(fileName,"%d",getpid());
	int fd = open(fileName, O_CREAT | O_RDWR, 0666);
	if (fd < 0) {
		perror("Error opening file");
		return fd;
	}

	char** inS = malloc(sizeof(char*)*3);
	inS[0] = malloc(sizeof(char) * IDSIZE);
	inS[1] = malloc(sizeof(char) * PRICESIZE);
	inS[2] = malloc(sizeof(char) * QUANTSIZE);

	int intNew[3];
	int i = 0;
	while (readLnC(0, inS, intNew) > 0) {
		int pos = getPos(fd,inS[0]);
		replace(fd, pos, intNew, inS);
		lseek(fd,0,SEEK_END);
		i++;
	}
	
	char c[AGRSIZE];
	lseek(fd,0L, SEEK_SET);
	while (read(fd,c,AGRSIZE) > 0){
		printf("%s",c);
	}
	if (remove(fileName) < 0) {
		perror("Could not delete file");
	}
	freeC(inS);
	free(fileName);
	return 1;
	
}
