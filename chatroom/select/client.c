#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define BUFFER_SIZE 128
#define SER_PORT 8003
#define SER_ADDR "127.0.0.1"

int main (int argc, char* argv[]){
	int sock_fd, result;
	struct sockaddr_in info;
	char buffer[BUFFER_SIZE];
	struct timeval timeout;
	fd_set readfds, testfds;

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

	FD_ZERO(&readfds);
	FD_SET(sock_fd, &readfds);
	FD_SET(0, &readfds);
	while(1){
		int nread;
		char *token;
		testfds = readfds;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout);
		switch(result){
			case -1:
				perror("client");
				exit(1);
			default:
				if(FD_ISSET(sock_fd, &testfds)){
					//printf("start----------------\n");
					ioctl(sock_fd, FIONREAD, &nread);
					read(sock_fd, buffer, nread);
					buffer[nread] = 0;
					//token = strtok(buffer, "+");
					printf("Client No.%s", buffer);
					//token = strtok(NULL, "+");
					//printf("%s\n", token);
					//printf("Client No.%s says:\n", buffer);
					//ioctl(sock_fd, FIONREAD, &nread);
					//read(sock_fd, buffer, nread);
					//printf("nread:    %d\n", nread);
					//buffer[nread] = 0;
					//printf("haha  %s\n",buffer);
				}else if(FD_ISSET(0, &testfds)){
					ioctl(0, FIONREAD, &nread);
					if(nread == 0){
						printf("keyboard done\n");
						exit(0);
					}
					nread = read(0, buffer, nread);
					buffer[nread] = 0;
					send(sock_fd, buffer, sizeof(buffer), 0);
				}
		}
		//printf("Input a string to send to server:\n");
		//scanf("%s",buffer);
		//send(sock_fd, buffer, sizeof(buffer), 0);
	}
	//recv(sock_fd, buffer, sizeof(buffer), 0);
	//printf("Receive string from server:\n%s\n",buffer);
	close(sock_fd);
	return 0;
}
