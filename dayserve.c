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

/* Associates and reserves a port for use by the socket */
void bindSocket(int listenFD)
{
    struct sockaddr_in servAddr; // Server Address

    //Create memory for server address to bind
    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;                //IPV4 Family
    servAddr.sin_port = htons(MY_PORT_NUMBER);    //Port Number
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Internet address
    if (bind(listenFD, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    {
        perror("Bind error: ");
        exit(EXIT_FAILURE);
    }
}

/* Instructs TCP protocol implementation to listen for connections */
void listenSocket(int listenFD)
{
    //Takes in Socket ID and active participants that can “wait” for a connection.
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

    //Takes Socket ID, Foreign Client address, Addres len size of name.
    //and return connection descriptor
    connectFD = accept(listenFD, (struct sockaddr *)&clientAddr, (socklen_t *)&length);
    if (connectFD < 0)
    {
        perror("Connect error: ");
        exit(EXIT_FAILURE);
    }

    //Get the host name
    hostEntry = gethostbyaddr(&(clientAddr.sin_addr), sizeof(struct in_addr), AF_INET);
    hostName = hostEntry->h_name;
    if (hostName == NULL)
    {
        herror("Host Name");
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

int main(int argc, char const *argv[])
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
            char *dateTime = getDateTime();
            if ((write(connectfd, dateTime, strlen(dateTime))) < 0)
            {
                perror("Write error: ");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
