#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100
#define SER_PORT 8003
#define SER_ADDR "127.0.0.1"

int main (int argc, char* argv[]){
	int sock_fd;
	struct sockaddr_in info;
	char buffer[BUFFER_SIZE];

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
	
	printf("Input a string to send to server:\n");
	scanf("%s",buffer);
	send(sock_fd, buffer, sizeof(buffer), 0);
	recv(sock_fd, buffer, sizeof(buffer), 0);
	printf("Receive string from server:\n%s\n",buffer);
	close(sock_fd);
	return 0;
}
