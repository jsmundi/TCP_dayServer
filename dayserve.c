/*
 * dayserve.c
 * 
 * JT Mundi
 * 
 * System Programming
 * Ben Mccamish
 * CS 360
 * 
 * Assignment 9
 * Day Server
 * 
 * Created: April 19, 2019
 * Modified: April 13, 2019
 * 
 * Server Logic
 * Socket -> Bind -> Listen -> Accept -> Receive -> Send -> Close
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MY_PORT_NUMBER 11899 //Dayserve port

/* Create the inteface for new communicatin end point */
int makeSocket()
{

    int tempListFD;

    /*
     * AF_INET communication domain (IPV4 Family)
     * SOCK_STREAM communciation typereliable, 2-way, connection-based service.
     * SOCK_STREAM ensures that data is not lost or duplicated.
     */
    tempListFD = socket(AF_INET, SOCK_STREAM, 0);

    //Check for socket error
    if (tempListFD < 0)
    {
        perror("Socket error: ");
        exit(EXIT_FAILURE);
    }

    return tempListFD;
}

/* Associates and reserves a port for use by the socket */
void bindSocket(int listenFD)
{
    struct sockaddr_in servAddr; // Server Address

    //Create memory for server address to bind
    memset(&servAddr, 0, sizeof(servAddr));

    /* 
     * The htons function converts a u_short from host to TCP/IP 
     * network byte order (which is big-endian).
     */
    servAddr.sin_family = AF_INET;                //IPV4 Family
    servAddr.sin_port = htons(MY_PORT_NUMBER);    //A 16-bit port number in Network Byte Order. Big Endian
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //A 32-bit IP address in Network Byte Order. Big Endian

    /*
     * The bind() function binds a unique local name to the socket with descriptor socket.
     * listenFD: The socket descriptor returned by a previous makeSocket() call.
     * servAddr: The pointer to a sockaddr structure containing the name that is to be bound to socket.
     * servAddr: The size of address in bytes.
     */
    if (bind(listenFD, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Bind error: ");
        exit(EXIT_FAILURE);
    }
}

/* Instructs TCP protocol implementation to listen for connections */
void listenSocket(int listenFD)
{
    /* 
     * listen completes the binding necessary for a socket listeFD, 
     * if bind() has not been called for s, and it creates a connection 
     * request queue of length backlog to queue incoming connection requests. 
     * When the queue is full, additional connection requests are ignored.
     */
    if (listen(listenFD, 1) < 0)
    {
        perror("Bind error: ");
        exit(EXIT_FAILURE);
    }
}

/* Establishes a connection with the server and client, when client calls connect*/
int connectSocket(int listenFD)
{
    char *hostName;
    int connectFD;
    struct hostent *hostEntry;
    struct sockaddr_in clientAddr;

    int length = sizeof(struct sockaddr_in);

    /* 
     * accept() a connection request from a client. 
     * The call accepts the first connection on its queue of pending connections. 
     * The accept() call creates a new socket descriptor with the same properties as listenFD
     * and returns it to the caller. If the queue has no pending connection requests, accept() blocks
     * the caller unless s is in nonblocking mode. 
     * 
     * listenFD: The socket descriptor.
     * clientAddr: The socket address of the connecting client that is filled by accept() before it returns. 
     * length: size in bytes of the storage pointed to by addr
     */
    connectFD = accept(listenFD, (struct sockaddr *)&clientAddr, (socklen_t *)&length);
    if (connectFD < 0)
    {
        perror("Connect error: ");
        exit(EXIT_FAILURE);
    }

    /* 
     * Resolve the host name related to IP address 
     * clientAddr.sin_addr: Points to an unsigned long value containing the address of the host
     */
    hostEntry = gethostbyaddr(&(clientAddr.sin_addr), sizeof(struct in_addr), AF_INET);
    if (hostEntry == NULL)
    {
        herror("Error Host Entry: ");
        exit(EXIT_FAILURE);
    }

    //The address of the official name of the host
    hostName = hostEntry->h_name;

    //Error check for host name
    if (hostName == NULL)
    {
        herror("Error Host Name: ");
        exit(EXIT_FAILURE);
    }

    //Print message
    printf("Connected to host name: %s at port %d\n", hostName, MY_PORT_NUMBER);

    return connectFD;
}

/* Return the current date and time*/
char *getDateTime()
{
    time_t currTimeDate;
    time(&currTimeDate);
    return ctime(&currTimeDate);
}

int main(void)
{
    int listenfd;     //Socket descriptor
    int connectfd;    //Connection descriptor
    pid_t processPID; //Fork process ID

    //Make socket
    listenfd = makeSocket();

    //Bind socket
    bindSocket(listenfd);

    //Start listening
    listenSocket(listenfd);

    printf("Server started now listening at port: %d\n", MY_PORT_NUMBER);

    //Keep listening for connection requests
    while (1)
    {
        //Make connection
        connectfd = connectSocket(listenfd);

        //Create a connection with client
        if (connectfd < 0)
        {
            perror("Connect error: ");
            exit(EXIT_FAILURE);
        }

        //Fork a new process
        if ((processPID = fork()) < 0)
        {
            perror("Fork error: ");
            exit(EXIT_FAILURE);
        }

        if (processPID)
        {
            //Close the connection
            close(connectfd);
        }
        else
        {
            //Get date and time
            char *dateTime = getDateTime();

            //Write to client
            if ((write(connectfd, dateTime, strlen(dateTime))) < 0)
            {
                perror("Write error: ");
                exit(EXIT_FAILURE);
            }

            //Exit
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
