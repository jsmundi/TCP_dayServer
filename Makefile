CC=gcc
COPTS=-g  -Wall -pedantic

all: Server Client
	
Server: dayserve.c 
	$(CC) $(COPTS) dayserve.c -o Server

Client: daytime.c 
	$(CC) $(COPTS) daytime.c -o Client

clean: 
	rm Client Server 
	rm -rf Client.dSYM
	rm -rf Server.dSYM

run: all
	./Client localhost