#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>

#define BUFFER_SIZE 128
#define SER_PORT 8003
#define SER_ADDR "127.0.0.1"

void *con_recv(void *argu);
void *con_send(void *argu);

int main (int argc, char* argv[]){
	int sock_fd;
	struct sockaddr_in info;
	char buffer[BUFFER_SIZE];
	pthread_t thread_recv, thread_send;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		printf("Fail to create a socket!\n");
	}

	bzero(&info, sizeof(info));
	info.sin_family = PF_INET;
	info.sin_addr.s_addr = inet_addr(SER_ADDR);
	info.sin_port = htons(SER_PORT);

	if(connect(sock_fd, (struct sockaddr*)&info, sizeof(info)) == -1){
		printf("Fail to connect!\n");
		close(sock_fd);
		exit(1);
	}

	printf("Connected!\n");
	pthread_create(&thread_recv, NULL, &con_recv, &sock_fd);
	pthread_create(&thread_send, NULL, &con_send, &sock_fd);
	if(pthread_join(thread_recv, NULL)){
		printf("Fail to join thread!\n");
	}
	//while(1){
		//printf("Input a strinig to send to server:\n");
		//scanf("%s",buffer);
		//send(sock_fd, buffer, sizeof(buffer), 0);
		//recv(sock_fd, buffer, sizeof(buffer), 0);
		//write(sock_fd, buffer, BUFFER_SIZE);
		//read(sock_fd, buffer, BUFFER_SIZE);
		//printf("Receive string from server:\n%s\n",buffer);
	//}
	//while(1){
		//recv(sock_fd, buffer, sizeof(buffer), 0);
		//printf("Receive string from client No.%d:\n", atoi(buffer));
		//recv(sock_fd, buffer, sizeof(buffer), 0);
		//printf("%s\n", buffer);
	//}
	//close(sock_fd);
	return 0;
}

void *con_recv(void *argu){
	int sock_fd = *((int *)argu);
	char buffer[BUFFER_SIZE];
	while(1){
		recv(sock_fd, buffer, sizeof(buffer), 0);
		printf("Client No.%d says:\n", atoi(buffer));
		recv(sock_fd, buffer, sizeof(buffer), 0);
		printf("%s\n", buffer);
	}
}
void *con_send(void *argu){
	int sock_fd = *((int *)argu);
	char buffer[BUFFER_SIZE];
	while(1){
		printf("Input a string to send to server!\n");
		scanf("%s", buffer);
		send(sock_fd, buffer, sizeof(buffer), 0);
	}
}
