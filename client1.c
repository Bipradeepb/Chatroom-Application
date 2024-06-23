#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "10.2.78.228"
#define PORT 8080
#define BUFFER_SIZE 1024

#define DOMAIN AF_INET
#define TYPE SOCK_STREAM
#define PROTOCOL 0

pthread_t send_thread,receive_thread;

void *send_message(void *arg)
{
    int client_socket=*((int *)arg);
    char message[BUFFER_SIZE];

    while(1)
    {
        if(fgets(message,BUFFER_SIZE,stdin)==NULL)
        {
            perror("Client error. Could not get input...\n");
            break;
        }
        ssize_t sendstatus=send(client_socket,message,strlen(message),0);
        if(sendstatus==-1)
        {
            perror("Could not send message to server.\n");
            break;
        }

        if(strncmp("'Bye'",message,3)==0)
        {
            printf("Disconnected from server %s...\n",SERVER_IP);
            break;
        }

        memset(message,'\0',sizeof message);
    }

    //cancel all threads
    pthread_cancel(receive_thread);
    pthread_exit(NULL);
}

void *receive_message(void *arg)
{
    int client_socket=*((int*)arg);
    char buffer[BUFFER_SIZE];

    while(1)
    {
        ssize_t receivestatus=recv(client_socket,buffer,sizeof buffer,0);

        if(receivestatus==-1)
        {
            perror("Could not receive message from the server...\n");
            break;
        }

        buffer[receivestatus]='\0';

        printf("Server says:%s",buffer);

        if(strncmp("'Bye'",buffer,3)==0)
        {
            printf("Disconnected from server %s...\n",SERVER_IP);
            break;
        }

        memset(buffer,'\0',sizeof buffer);
    }

    //cancel all threads 
    pthread_cancel(send_thread);
    pthread_exit(NULL);
}


void err(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int main()
{
    int client_socket,server_socket;

    //create the client socket
    client_socket=socket(DOMAIN,TYPE,PROTOCOL);
    if(client_socket==-1)
    {
        err("Could not create client socket...\n");
    }

    struct sockaddr_in server_addr;
    socklen_t serverlen=sizeof server_addr;

    //configure socket settings
    server_addr.sin_family=DOMAIN;
    server_addr.sin_port=htons(PORT);
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr)<=0)
    {
        err("Invalid ip...\n");
    }

    //connect to the server
    server_socket=connect(client_socket,(struct sockaddr*)&server_addr,serverlen);
    if(server_socket==-1)
    {
        err("Could not connect to server.\n");
    }

    printf("Connected to server at %s.\n",SERVER_IP);
    printf("Type 'Bye' to end session.\n");

    if(pthread_create(&send_thread,NULL,(void *)send_message,(void *)&client_socket))
    {
        perror("Send thread creation failed...\n");
        goto label;
    }

    if(pthread_create(&receive_thread,NULL,(void *)receive_message,(void *)&client_socket))
    {
        perror("Receive thread creation failed...\n");
        goto label;
    }

    pthread_join(receive_thread,NULL);
    pthread_join(send_thread,NULL);

label:
    sleep(2);
    printf("Closing client socket...\n");
    close(client_socket);

    exit(0);

}