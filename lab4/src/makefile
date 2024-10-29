CC=gcc
CFLAGS=-I.

all : parallel_mm process_memory

parallel_mm : find_min_max.c find_min_max.h utils.c utils.h
	$(CC) -o parallel_mm parallel_min_max.c find_min_max.c find_min_max.h utils.c utils.h $(CFLAGS)

process_memory : process_memory.c
	$(CC) -o process_memory process_memory.c $(CFLAGS)

clean :
	rm -f parallel_mm process_memory