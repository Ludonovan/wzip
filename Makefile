wzip: wzip.o 
	gcc -o wzip wzip-mthread.o

wzip.o: wzip.c
	gcc -c wzip-mthread.c

clean:
	rm wzip wzip.o
	clear
