#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<netinet/in.h>
#include<unistd.h>
#include<pthread.h>

#define BUFFER_SIZE 128
#define SER_PORT 8003
#define SER_ADDRESS "127.0.0.1"
#define THREAD_NUM 3
#define STR_BUFFER 10

void *client(void *argu);

char string[STR_BUFFER][128];
int who[STR_BUFFER]={0};
int str_index=0;

int main(int argc, char* argv[]){
	char buffer[BUFFER_SIZE];
	int ser_fd, cli_fd;
	struct sockaddr_in ser_addr, cli_addr;
	int addr_len = sizeof(ser_addr);
	pthread_t thread_client[THREAD_NUM];
	int thread_index=0;

	ser_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(ser_fd == -1){
		printf("Fail to create server socket!\n");
		exit(1);
	}

	bzero(&ser_addr, addr_len);
	ser_addr.sin_family = PF_INET;
	ser_addr.sin_addr.s_addr = INADDR_ANY;
	ser_addr.sin_port = htons(SER_PORT);

	if(bind(ser_fd, (struct sockaddr*)&ser_addr, addr_len) == -1){
		printf("Fail to bind!\n");
		exit(1);
	}

	if(listen(ser_fd, 1) == -1){
		printf("Fail to listen!\n");
		close(ser_fd);
		exit(1);
	}

	while(1){
		printf("Listening!\n");
		cli_fd = accept(ser_fd, (struct sockaddr*)&cli_addr, &addr_len);
		printf("Client No.%d connected!\n", cli_fd);
		//read(cli_fd, buffer, BUFFER_SIZE);
		//printf("Receive string:\n%s\n\n",buffer);
		//strcpy(buffer, "Welcome!\n");
		//write(cli_fd, buffer, BUFFER_SIZE);
		pthread_create(&thread_client[thread_index++], NULL, &client, &cli_fd);
	}
	return 0;
}

void *client(void *argu){
	int cli_fd = *((int *)argu);
	char buffer[BUFFER_SIZE];
	int local_str_index = str_index;
	//while(1){
		//if(local_str_index != str_index){
			//send message to client
		//}
		recv(cli_fd, buffer, sizeof(buffer), 0);
		printf("Receive string from client No.%d:\n%s\n\n", cli_fd, buffer);
		strcpy(string[str_index], buffer);
		who[str_index] = cli_fd;
		str_index = (str_index+1)%STR_BUFFER;
		//local_str_index = (local_str_index+1)%10;
		strcpy(buffer, "Welcome!\n");
		send(cli_fd, buffer, sizeof(buffer), 0);
	//}
	while(1){
		if(local_str_index != (str_index-1)){
			local_str_index = (local_str_index+1)%STR_BUFFER;
			sprintf(buffer, "%d", who[local_str_index]);
			send(cli_fd, buffer, sizeof(buffer), 0);
			send(cli_fd, string[local_str_index], sizeof(string[local_str_index]), 0);
		}
	}
	return 0;
}
