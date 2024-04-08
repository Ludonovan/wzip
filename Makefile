wzip: wzip.o 
	gcc -o wzip wzip.o

wzip.o: wzip.c
	gcc -c wzip.c

clean:
	rm wzip wzip.o
	clear
