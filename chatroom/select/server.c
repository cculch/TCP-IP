#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<fcntl.h>

#define BUFFER_SIZE 128
#define SER_PORT 8003
#define SER_ADDR "127.0.0.1"
#define CLIENT_NUM 3

int main (int argc, char* argv[]){
	char buffer[BUFFER_SIZE], tmp[BUFFER_SIZE];
	int ser_fd, cli_fd;
	struct sockaddr_in ser_info,cli_info;
	int addr_len = sizeof(cli_info);
	int result;
	fd_set readfds, testfds;
	struct timeval timeout;
	int who[CLIENT_NUM]={0};
	int cli_index = 0;

	ser_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(ser_fd == -1){
		printf("Fail to create a socket!\n");
		exit(1);
	}

	bzero(&ser_info, sizeof(ser_info));
	ser_info.sin_family = PF_INET;
	ser_info.sin_addr.s_addr = INADDR_ANY;
	ser_info.sin_port = htons(SER_PORT);

	if(bind(ser_fd, (struct sockaddr*)&ser_info, sizeof(ser_info)) == -1){
		printf("Fail to bind!\n");
		close(ser_fd);
		exit(1);
	}

	if(listen(ser_fd, 1) == -1){
		printf("Fail to listen!\n");
		close(ser_fd);
		exit(1);
	}

	//printf("%d\n", ser_fd);

	FD_ZERO(&readfds);
	FD_SET(ser_fd, &readfds);

	while(1){
		int nread;
		testfds = readfds;
		printf("Listening!\n");
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, &timeout);
		
		if(result == 0){
			continue;
		}		
		
		for(int fd = 0; fd<FD_SETSIZE; fd++){
			if(FD_ISSET(fd, &testfds)){
				if(fd == ser_fd){
					cli_fd = accept(ser_fd, (struct sockaddr*)&cli_info, &addr_len);
					FD_SET(cli_fd, &readfds);
					printf("Add client No.%d!\n", cli_fd);
					who[cli_index++] = cli_fd;
				}else{
					ioctl(fd, FIONREAD, &nread);
					if(nread == 0){
						close(fd);
						FD_CLR(fd, &readfds);
						printf("remove client No.%d!\n", fd);
					}else{
						read(fd, buffer, nread);
						buffer[nread] = 0;
						printf("Receive message from client No.%d:\n%s\n", fd, buffer);
						//send to other client
						for(int i=0; i<cli_index; i++){
							if((who[i] != fd) && (who[i] != 0)){
								sprintf(tmp, "%d", fd);
								strcat(tmp, " says:\n");
								strcat(tmp, buffer);
								send(who[i], tmp, sizeof(tmp), 0);
								//send(who[i], buffer, sizeof(buffer), 0);
							}
						}
					}
				}
			}
		}
		//client_sock_fd = accept(sock_fd, (struct sockaddr*)&cli_info, &addr_len);
		//recv(client_sock_fd, buffer, sizeof(buffer), 0);
		//printf("Receive string:\n%s\n\n",buffer);
		//strcpy(buffer,"Welcome!");
		//send(client_sock_fd, buffer, sizeof(buffer), 0);
	}

	return 0;
}
