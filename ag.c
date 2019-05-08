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
typedef void(*sighandler_t)(int);

#define COMMANDSIZE 1
#define NAMESIZE 80
#define ID 5
#define PRICE 4
#define QTD 3
#define LINE ID + 1 + PRICE + 1 + QTD + 1

/*
	printf("[%s] [%s] [%s]\n",inString[0], inString[1], inString[2]);
	
	printf("%d %d %d\n",inInt[0],inInt[1], inInt[2]);
*/

int readLnC(int fd, char** inS, int* in) {	
	
	read(fd, inS[0], ID);
	lseek(fd, 1, SEEK_CUR);
	read(fd, inS[1], PRICE);
	lseek(fd, 1, SEEK_CUR);
	int r = read(fd, inS[2], QTD);
	lseek(fd, 1, SEEK_CUR);

	in[0] = atoi(inS[0]);
	in[1] = atoi(inS[1]);
	in[2] = atoi(inS[2]);

	return r;
}

int readLn(int fd, int* in) {	

	char** inS = malloc(sizeof(char*)*3);
	inS[0] = malloc(sizeof(char) * ID);
	inS[1] = malloc(sizeof(char) * PRICE);
	inS[2] = malloc(sizeof(char) * QTD);

	read(fd, inS[0], ID);
	lseek(fd, 1, SEEK_CUR);
	read(fd, inS[1], PRICE);
	lseek(fd, 1, SEEK_CUR);
	int r = read(fd, inS[2], QTD);
	lseek(fd, 1, SEEK_CUR);

	in[0] = atoi(inS[0]);
	in[1] = atoi(inS[1]);
	in[2] = atoi(inS[2]);
	//free(inS[0]);
	//free(inS[1]);
	//free(inS[2]);
	//free(inS);

	return r;
}

off_t getPos(int fd, char* id) {

	off_t end = lseek(fd, 0L, SEEK_END);
	lseek(fd, 0L, SEEK_SET);
	off_t r = 0;
	while(r < end){
		lseek(fd,r,SEEK_SET);
		char* c = malloc(sizeof(char)* ID);
		read(fd,c,ID);
		if (strcmp(c,id) == 0) {
			
			return r;
			lseek(fd,0,SEEK_SET);
		}
		r += LINE;
	}
	return r;
	lseek(fd,0,SEEK_SET);
}

void test(FILE* f, off_t pos) {
	
	char c[16] = "hello i am gay\n";
	fseek(f,pos,SEEK_SET);
	
	fwrite(c,1,15,f);
}

void ftest(int fd, FILE* f, off_t pos, int* in, char** s) {
	
	lseek(fd,pos,SEEK_SET);
	int old[3];
	old[1] = 0;
	old[2] = 0;
	readLn(fd, old);
	
	printf("hm-> %d + %d = %d\n",in[1], old[1], in[1] + old[1]);
	printf("hm-> %d + %d = %d\n",in[2], old[2], in[2] + old[2]);
	
	in[1] += old[1];
	in[2] += old[2];
	
	char c[LINE + 1];
	sprintf(s[1],"%0*d", PRICE, in[1]);
	sprintf(s[2],"%0*d", QTD, in[2]);

	printf("hm-> %s %s\n",s[1], s[2]);
	
	fseek(f,pos,SEEK_SET);
	fwrite(s[0],1,ID,f);
	fwrite(" ",1,1,f);
	fwrite(s[1],1,PRICE,f);
	fwrite(" ",1,1,f);
	fwrite(s[2],1,QTD,f);
	fwrite("\n",1,1,f);
	
}

int main(int argc, char* argv[]) {
	//sprintf(buf,"%0*d\n", ID, 36182);
	int fd = open("tmp", O_CREAT | O_RDWR, 0666);
	if (fd < 0) {
		perror("Error opening file");
		return fd;
	}
	FILE *f = fdopen(fd, "r+");

	char** inS = malloc(sizeof(char*)*3);
	inS[0] = malloc(sizeof(char) * ID);
	inS[1] = malloc(sizeof(char) * PRICE);
	inS[2] = malloc(sizeof(char) * QTD);

	int intNew[3];
	int i = 0;
	while (readLnC(0, inS, intNew) > 0) {
		int pos = getPos(fd,inS[0]);
		printf("%d\n",pos);
		
		ftest(fd, f,pos, intNew, inS);
		fseek(f,0,SEEK_END);
		i++;
	}
	
	char c[LINE];
	lseek(fd,0L, SEEK_SET);
	while (read(fd,c,LINE) > 0){
		printf("%s",c);
	}
	if (remove("tmp") < 0) {
		perror("Could not delete file");
	}
	return 1;
	
}