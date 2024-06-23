#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define PORT 8080
#define BUFFER_SIZE 1024


#define DOMAIN AF_INET
#define TYPE SOCK_STREAM
#define PROTOCOL 0

typedef struct
{
    int clientsockfd;
    char name[100];
    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in client_addr;
} Client;

Client *clients[BACKLOG+1];

//declare threads 
pthread_t send_thread,receive_thread;
int clientstatus;

void err(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

void *send_message(void* arg)
{
    int client_socket=*((int *)arg);
    char message[BUFFER_SIZE];

    //send messages in a loop

    while(1)
    {
        if(fgets(message,BUFFER_SIZE,stdin)==NULL)
        {
            perror("Server error. Can't get input");
            break;
        }

        ssize_t sendstatus=send(client_socket,message,strlen(message),0);

        if(sendstatus==-1)
        {
            perror("Server could not send to client.Shutting down...\n");
            break;
        }

        if(strncmp("'Bye'",message,3)==0)
        {
            clientstatus=0;
            break;
        }

        memset(message,'\0',sizeof message);
    }

    //Cancel the other thread
    pthread_cancel(receive_thread);
    pthread_exit(NULL);
}

void *receive_message(void *arg)
{
    int client_socket=*((int *)arg);
    char buffer[BUFFER_SIZE];
    memset(buffer,'\0',sizeof buffer);

    while(1)
    {
        ssize_t receivestatus=recv(client_socket,buffer,BUFFER_SIZE,0);
        if(receivestatus==-1)
        {
            perror("Server could not get messages from client. Shutting down...\n");
            break;
        }

        buffer[receivestatus]='\0';
        printf("Client says:%s",buffer);

        if(strncmp("'Bye'",buffer,3)==0)
        {
            clientstatus=-1;
            break;
        }
    }

    //close all threads
    pthread_cancel(send_thread);
    pthread_exit(NULL);
}

int main()
{
    int server_socket,client_socket;
    struct sockaddr_in server_addr,client_addr;

    //Create a socket
    server_socket=socket(DOMAIN,TYPE,PROTOCOL);
    if(server_socket==-1)
    {
        err("Problem creating socket");
    }

    //configure the server_socket
    server_addr.sin_family=DOMAIN;
    server_addr.sin_port=htons(PORT);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //bind the socket
    int bindstatus=bind(server_socket,(struct sockaddr*)&server_addr,sizeof server_addr);
    if(bindstatus==-1)
    {
        err("Server Bind() failed...\n");
    }

    //listen
    int listenstatus=listen(server_socket,1);
    if(listenstatus==-1)
    {
        err("Server listen failed...\n");
    }

clientconnection:
    printf("Server is listening at port %d.\n",PORT);
    //accept
    socklen_t clientlen=sizeof client_addr;
    client_socket=accept(server_socket,(struct sockaddr*)&client_addr,&clientlen);
    if(client_socket==-1)
    {
        err("Failed to accept client...\n");
    }

    char client_ip[INET_ADDRSTRLEN]; //store client ip
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("Client connected successfully! IP: %s\n", client_ip);
    printf("Type 'Bye' to end session.\n");

    clientstatus=1;

    if(pthread_create(&send_thread,NULL,(void *)&send_message,(void *)&client_socket))
    {
        perror("Send thread creation failed...\n");
        goto label;
    }

    if(pthread_create(&receive_thread,NULL,(void *)&receive_message,(void *)&client_socket))
    {
        perror("Receive thread creation failed...\n");
        pthread_cancel(send_thread);
        goto label;
    }

    pthread_join(receive_thread,NULL);
    pthread_join(send_thread,NULL);

    //check clientstatus
    if(clientstatus==-1)
    {
        //client disconnected. server still listening
        sleep(2);
        printf("Closing client socket...\n");
        printf("Disconnected from client IP:%s\n",client_ip);
        close(client_socket);
        goto clientconnection;
    }

    //close the server and client socket
label:
    sleep(2);
    printf("Closing client socket...\n");
    close(client_socket);
    sleep(2);
    printf("Closing server socket...\n");
    close(server_socket);

    exit(0);

}
