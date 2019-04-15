CC=gcc
COPTS=-g  -Wall -pedantic

Server: dayserve.c Client
	$(CC) $(COPTS) dayserve.c -o Server

Client: daytime.c Server
	$(CC) $(COPTS) daytime.c -o Client