#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 100
#define SER_PORT 8003
#define SER_ADDR "127.0.0.1"

int main (int argc, char* argv[]){
	char buffer[BUFFER_SIZE];
	int sock_fd, client_sock_fd;
	struct sockaddr_in ser_info,cli_info;
	int addr_len = sizeof(cli_info);

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1){
		printf("Fail to create a socket!\n");
		exit(1);
	}

	bzero(&ser_info, sizeof(ser_info));
	ser_info.sin_family = PF_INET;
	ser_info.sin_addr.s_addr = INADDR_ANY;
	ser_info.sin_port = htons(SER_PORT);

	if(bind(sock_fd, (struct sockaddr*)&ser_info, sizeof(ser_info)) == -1){
		printf("Fail to bind!\n");
		close(sock_fd);
		exit(1);
	}

	if(listen(sock_fd, 1) == -1){
		printf("Fail to listen!\n");
		close(sock_fd);
		exit(1);
	}

	while(1){
		printf("Listening!\n");
		client_sock_fd = accept(sock_fd, (struct sockaddr*)&cli_info, &addr_len);
		recv(client_sock_fd, buffer, sizeof(buffer), 0);
		printf("Receive string:\n%s\n\n",buffer);
		strcpy(buffer,"Welcome!");
		send(client_sock_fd, buffer, sizeof(buffer), 0);
	}

	return 0;
}
