#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>

#define SER_PORT 8002
#define BUFFER_SIZE 100

int main(int argc, char* argv[]){
	struct sockaddr_in info;
	int sock_fd, byte_recv;
	char buffer[BUFFER_SIZE];
	int info_len = sizeof(info);

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == -1){
		printf("Fail to create a socket!\n");
		exit(1);
	}

	bzero(&info, sizeof(info));
	info.sin_family = AF_INET;
	info.sin_port = htons(SER_PORT);
	info.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(sock_fd, (struct sockaddr*)&info, sizeof(info)) == -1){
		printf("Fail to bind!\n");
		close(sock_fd);
		exit(1);
	}
	while(1){
		printf("Listening!\n");
		struct sockaddr_in client_info;
		socklen_t client_info_len = sizeof(client_info); 
		bzero(buffer, BUFFER_SIZE);
		if(recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_info, &client_info_len) < 0){
			printf("Receive data fail!\n");
		}
		else{
			printf("Receive from client:\n");
			printf("%s\n", buffer);

			strcpy(buffer,"Welcome!");
			if(sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_info, sizeof(client_info)) == -1){
				printf("Fail to reply!\n");
			}
		}
	}
	return 0;

}
