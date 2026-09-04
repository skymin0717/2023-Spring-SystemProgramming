CC     = gcc
CFLAGS = -Wall -g

all: client server

client: client_main.c client.c ftp_test.h
	$(CC) $(CFLAGS) -o client client_main.c client.c

server: server.c ftp_test.h
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -f client server

.PHONY: all clean
