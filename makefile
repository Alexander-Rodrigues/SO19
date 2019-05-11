make:
	gcc -o ma ma.c functions.c
	gcc -o sv sv.c functions.c
	gcc -o cv cv.c functions.c
	gcc -o ag ag.c functions.c

clean:
	rm -f ma sv cv fifo/*

clean*:
	rm -f ma sv cv ag fifo/* files/*