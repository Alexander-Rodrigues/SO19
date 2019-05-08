make:
	gcc -o ma ma.c functions.c
	gcc -o sv sv.c functions.c
	gcc -o cv cv.c functions.c

clean:
	rm -f ma sv cv fifo* ARTIGOS.txt VENDAS.txt