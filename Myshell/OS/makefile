CC=gcc
myshell: code.o built.o header.h
	$(CC) -o myshell code.o built.o -I.
code.o: code.c built.o header.h
	$(CC) -c code.c -I.
built.o: built.c header.h
	$(CC) -c built.c -I.

.PHONY: clean

clean:
	rm -f *.o
