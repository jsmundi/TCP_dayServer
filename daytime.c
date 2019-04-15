/*
 * daytime.c
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
 * Socket -> Connect -> Send -> Receive -> Close
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

#define MY_PORT_NUMBER 49999 //Dayserve port

/* Create the inteface for new communicatin end point */
int makeSocket()
{

    int tempListFD;

    /*
     * AF_INET communication domain (IPV4 Family)
     * SOCK_STREAM communciation typereliable, 2-way, connection-based service
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

void setUpServer(int socketFD, char *userHost)
{
    int connectFD;
    struct sockaddr_in servAddr;
    struct hostent *hostEntry;
    struct in_addr **pptr;

    //Set the memory
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);

    //Get host name
    hostEntry = gethostbyname(userHost);
    if (hostEntry == NULL)
    {
        herror("Host Name");
        exit(EXIT_FAILURE);
    }

    pptr = (struct in_addr **)hostEntry->h_addr_list;
    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr));

    if ((connectFD = (connect(socketFD, (struct sockaddr *)&servAddr, sizeof(servAddr)))) < 0)
    {
        perror("Connection error: ");
        exit(1);
    }
}

int main(int argc, char const *argv[])
{
    /* Make an Internet socket using TCP protocol */
    int socketfd;
    int connectfd;
    char serverResponse[512];

    if (argc != 3)
    {
        perror("Not enough arguments: ");
        exit(EXIT_FAILURE);
    }
    else
    {
        socketfd = makeSocket();
        setUpServer(socketfd, argv[1]);
        read(socketfd, serverResponse, 512);
        printf("%s", serverResponse);
        close(socketfd);
        return 0;
    }
}
