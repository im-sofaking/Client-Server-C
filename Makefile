CC = gcc
CFLAGS = -Wall

all: server.out client.out

server.out: Server.c
	$(CC) $(CFLAGS) Server.c -o server.out

client.out: Client.c
	$(CC) $(CFLAGS) Client.c -o client.out

clean:
	rm -f server.out client.out

