wzip: wzip.o
	gcc -o wzip wzip.c -Wall -Werror

wzip.o: wzip.c
	gcc wzip.c

clean:
	rm wzip.o
