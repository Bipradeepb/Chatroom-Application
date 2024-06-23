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

#define PORT 8080

struct sockaddr_in server_addr;
socklen_t addr_size;
int sockfd;
char s[INET6_ADDRSTRLEN];
pthread_t sender,receiver;

void* sendMessage(){

    while(1){
        char msg[500];
        bzero(msg,sizeof msg);
		fgets(msg,sizeof msg,stdin);
		if(strlen(msg)<3){
			printf("Wrong Message Format!\n");
			continue;
		}
		send(sockfd,msg,(strlen(msg)+1)*sizeof(char),0);
    }

    pthread_exit(NULL);
}

void* recvMessage(){

    while(1){
        char msg[500];
		bzero(msg,sizeof msg);
		int status = recv(sockfd,&msg,sizeof msg,0);
		if(!status){
			close(sockfd);
			printf("\nServer got terminated\n");
			pthread_cancel(sender);
			pthread_exit(NULL);
		}
		printf("%s",msg);
    }
}


void *get_in_addr(struct sockaddr* sa){
	return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(int argc, char *argv[]){

	
	sockfd= socket(AF_INET, SOCK_STREAM, 0);
	addr_size=sizeof(server_addr);
	
	server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    inet_pton(AF_INET,"127.0.0.1", &server_addr.sin_addr);

	connect(sockfd, (struct sockaddr*)&server_addr, addr_size);
	
	char s[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET,&server_addr.sin_addr,s,sizeof s);

	printf("Use format \"<receiver_client_id> <message>\". Client ID ranges from 1 to 9.\n");
    pthread_create(&sender,NULL, sendMessage, NULL);

    int status;
    pthread_create(&receiver,NULL,recvMessage,NULL);

    while(pthread_join(receiver,NULL)!=0){
    }


	printf("\nClient is now closing....\n");
	close(sockfd);
	pthread_exit(NULL);
		
}

