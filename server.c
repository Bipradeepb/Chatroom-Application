#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define DOMAIN AF_INET
#define TYPE SOCK_STREAM
#define PROTOCOL 0

#define PORT 5555
#define BUFFER_SIZE 1024

int main()
{
    int server_socket,client_socket;

    //create a socket
    server_socket=socket(DOMAIN,TYPE,PROTOCOL);
    if(server_socket==-1)
    {
        perror("There was an error creating the socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sockaddr,clientaddr;

    //configure server socket address

    sockaddr.sin_family=DOMAIN;
    sockaddr.sin_port=htons(PORT);
    sockaddr.sin_addr.s_addr=INADDR_ANY;
    memset(sockaddr.sin_zero, '\0', sizeof sockaddr.sin_zero);

    //bind the status
    int bindstatus=bind(server_socket, (struct sockaddr *)&sockaddr, sizeof sockaddr);
    if(bindstatus==-1)
    {
        perror("Bind() failed..\n");
        exit(EXIT_FAILURE);
    }

    //start listen()ing 

    int listenstatus=listen(server_socket,1);
    if(listenstatus==-1)
    {
        perror("Listen() failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on PORT %d...\n",PORT);

    //now ready to accept client connections
    //accept(serversocketFD,struct sockaddr* &clientaddr,socklen_t clientaddr)
    socklen_t clientlen=sizeof(clientaddr);
    client_socket=accept(server_socket,(struct sockaddr*)&clientaddr,&clientlen);
    if(client_socket==-1)
    {
        perror("Accept() failed..\n");
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip, sizeof(client_ip));
    printf("Client connected successfully! IP: %s\n", client_ip);

    char buffer[BUFFER_SIZE];

    while(1)
    {
        //receive messages from the client;
        ssize_t receivestatus=recv(client_socket,buffer,sizeof(buffer),0);
        if(receivestatus==-1)
        {
            perror("Receive from client failed..\n");
            break;
        }

        buffer[receivestatus]='\0';

        printf("Client says:%s",buffer);

        if(strncmp("Bye",buffer,3)==0)
        {
            break;
        }


        //clear the buffer
        memset(buffer,'\0',sizeof(buffer));

        //send messages to client
        printf("Enter your message:");
        if(fgets(buffer,BUFFER_SIZE,stdin)==0)
        {
            perror("Failed to get input..\n");
            break;
        }

        ssize_t sendstatus=send(client_socket,buffer,strlen(buffer),0);
        if(sendstatus==-1)
        {
            perror("Send to client failed..\n");
            break;
        }

        if(strncmp("Bye",buffer,3)==0)
        {
            break;
        }

        memset(buffer,'\0',sizeof(buffer));
    }

    sleep(1);
    printf("Closing client socket...\n");
    close(client_socket);
    sleep(3);
    printf("Closing server socket...\n");
    close(server_socket);
    sleep(3);

    return 0;
}