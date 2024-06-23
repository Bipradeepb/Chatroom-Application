#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define DOMAIN AF_INET
#define TYPE SOCK_STREAM
#define PROTOCOL 0

#define PORT 5555
#define SERVER_IP "10.2.78.228"
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;

    //create a socket
    sockfd=socket(DOMAIN,TYPE,PROTOCOL);
    if(sockfd==-1)
    {
        perror("There was an error creating the socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serveraddr;

    //configure the server details
    serveraddr.sin_family=DOMAIN;
    serveraddr.sin_port=htons(PORT);
    if(inet_pton(AF_INET, SERVER_IP, &serveraddr.sin_addr)<=0)
    {
        perror("Invalid IP..\n");
        exit(EXIT_FAILURE);
    }

    //connect to the server

    int connectstatus=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    if(connectstatus==-1)
    {
        perror("Connect() failed...\n");
        exit(EXIT_FAILURE);
    }

    printf("Connected to Server at %s.Client process started..\n",SERVER_IP);

    char buffer[BUFFER_SIZE];

    while(1)
    {
        printf("Enter message:");

        if(fgets(buffer,BUFFER_SIZE,stdin)==NULL)
        {
            perror("Error occured while reading input..\n");
            exit(EXIT_FAILURE);
        }

        //send the message to server
        ssize_t sendstatus=send(sockfd,buffer,strlen(buffer),0);

        if(sendstatus==-1)
        {
            perror("Send() to server failed..\n");
            exit(EXIT_FAILURE);
        }

        if(strncmp("Bye",buffer,3)==0)
        {
            break;
        }


        //clear the buffer
        memset(buffer,'\0',sizeof(buffer));

        //receive the message from the server
        ssize_t receivestatus=recv(sockfd,buffer,BUFFER_SIZE,0);
        
        if(receivestatus==-1)
        {
            perror("Receive() from server failed..\n");
            exit(EXIT_FAILURE);
        }

        buffer[receivestatus]='\0';

        printf("Server says: %s",buffer);

        if(strncmp("Bye",buffer,3)==0)
        {
            break;
        }

        memset(buffer,'\0',sizeof(buffer));
    }

    printf("Closing client socket...\n");
    sleep(2);
    close(sockfd);

    return 0;

}