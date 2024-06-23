#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <error.h>

#define BAKCLOG 10	
#define PORT 8080	  

struct Client{
	char ip[INET6_ADDRSTRLEN];
    int id;
    int sockfd;
	pthread_t receiverThread;
};

struct msg{
	char message[500];
	int from;
	int to;
	int sockfd; //this is socket descriptor used to send from server to receiver client
};

struct receiveThreadMsg{
	int owner;
	int sockfd;
	struct msg* currMsg;
};

struct Client *ClientArr[BAKCLOG];
int currConnections=0;

void* recvMessage(void* data){
	
	int sockfd = ((struct receiveThreadMsg*)data)->sockfd;
	int owner = ((struct receiveThreadMsg*)data)->owner;
	printf("Reciver for %d active\n",owner);
    while(1){
		
		((struct receiveThreadMsg*)data)->currMsg->from=owner;
		bzero(((struct receiveThreadMsg*)data)->currMsg->message,sizeof(((struct receiveThreadMsg*)data)->currMsg->message));
		
		recv(sockfd,((struct receiveThreadMsg*)data)->currMsg->message,sizeof (((struct receiveThreadMsg*)data)->currMsg->message),0);

		((struct receiveThreadMsg*)data)->currMsg->to=((struct receiveThreadMsg*)data)->currMsg->message[0]-'0';
		((struct receiveThreadMsg*)data)->currMsg->sockfd=ClientArr[((struct receiveThreadMsg*)data)->currMsg->to-1]->sockfd;
		
		
		size_t len = strlen(((struct receiveThreadMsg*)data)->currMsg->message);
		for(size_t i=2;i<len;i++){
			((struct receiveThreadMsg*)data)->currMsg->message[i-2] = ((struct receiveThreadMsg*)data)->currMsg->message[i];
		}
		((struct receiveThreadMsg*)data)->currMsg->message[len-2] = '\0';
	

		printf("From: %d  To: %d  Message: %s",owner,((struct receiveThreadMsg*)data)->currMsg->to,((struct receiveThreadMsg*)data)->currMsg->message);

		char sendMsg[500];
		sprintf(sendMsg,"%d",owner);
		strcat(sendMsg,": ");
		strcat(sendMsg, ((struct receiveThreadMsg*)data)->currMsg->message);
		printf("Attempting to send...\n");
		send(((struct receiveThreadMsg*)data)->currMsg->sockfd,sendMsg,strlen(sendMsg)*sizeof(char),0);
		printf("Sent!\n\n");
	}
}



void *get_in_addr(struct sockaddr* sa){
	return &(((struct sockaddr_in*)sa)->sin_addr);
}



int main(){

	struct sockaddr_in server_addr,client_addr;
	socklen_t addr_size;
	int sockfd,new_fd;

	sockfd= socket(AF_INET,SOCK_STREAM,0);
	printf("Server started on port %d\n", PORT);

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(PORT);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof server_addr);
	listen(sockfd,BAKCLOG);
	

	while(currConnections!=BAKCLOG){
		
		addr_size = sizeof client_addr;
		new_fd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size);
		send(new_fd,"Hello! You are connected to the server\n",40*sizeof(char),0);
		currConnections++;
		
		struct Client *currClient= (struct Client*)(malloc(sizeof(struct Client)));
		currClient->sockfd=new_fd;
		currClient->id=currConnections;
		inet_ntop(AF_INET,&client_addr.sin_addr,currClient->ip,sizeof currClient->ip);
		
		printf("Server got connection from : %s and its id is %d\n",currClient->ip,currClient->id);
		
		ClientArr[currConnections-1] = currClient;

		struct receiveThreadMsg *currArgs=(struct receiveThreadMsg*)(malloc(sizeof(struct receiveThreadMsg)));
		currArgs->sockfd=new_fd;
		currArgs->owner=currClient->id;
		currArgs->currMsg = (struct msg*)malloc(sizeof(struct msg));

		pthread_create(&(currClient->receiverThread),NULL,recvMessage,(void *)(currArgs));
	}

	while(1){}

	close(sockfd);
	printf("\nServer is now closing....\n");
	pthread_exit(NULL);
		
}
