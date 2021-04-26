CC=gcc
CFLAGS=-I.

all: compileall

compileall:
	$(CC) main.c reader.c -o program
