CC=gcc
CFLAGS=-I.

all : client server

client : client.c
	$(CC) client.c -L. -lfactorial -o client -lpthread $(CFLAGS)

server : server.c
	$(CC) server.c -L. -lfactorial -o server -lpthread $(CFLAGS)

clean :
	rm -f client server