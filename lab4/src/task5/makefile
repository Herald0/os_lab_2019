CC=gcc
CFLAGS=-I.

all : parallel_sum

parallel_sum : parallel_sum.o sum.o utils.o
	$(CC) parallel_sum.o sum.o utils.o -o parallel_sum

parallel_sum.o : parallel_sum.c
	$(CC) -c parallel_sum.c -o parallel_sum.o $(CFLAGS)

sum.o : sum.c sum.h
	$(CC) -c sum.c -o sum.o $(CFLAGS)

utils.o : utils.c utils.h
	$(CC) -c utils.c -o utils.o $(CFLAGS)

clean :
	rm -f parallel_sum parallel_sum.o sum.o utils.o