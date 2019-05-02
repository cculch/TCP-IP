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
#define STR_BUFFER 50

void *client_recv(void *argu);
void *client_send(void *argu);
int if_cli_con(int cli_fd);
void cli_rm(int cli_fd);

char string[STR_BUFFER][128];
int who[STR_BUFFER]={0};
int str_index=0;
int connected_client[THREAD_NUM]={0};
int con_cli_index=0;

int main(int argc, char* argv[]){
	char buffer[BUFFER_SIZE];
	int ser_fd, cli_fd;
	struct sockaddr_in ser_addr, cli_addr;
	int addr_len = sizeof(ser_addr);
	pthread_t thread_client[THREAD_NUM*2];
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
		connected_client[con_cli_index++] = cli_fd; 
		pthread_create(&thread_client[thread_index*2], NULL, &client_recv, &cli_fd);
		pthread_create(&thread_client[(thread_index*2)+1], NULL, &client_send, &cli_fd);
		thread_index++;
	}
	return 0;
}

void *client_recv(void *argu){
	int cli_fd = *((int *)argu);
	char buffer[BUFFER_SIZE];

	while(1){
		if(recv(cli_fd, buffer, sizeof(buffer), 0) == 0){
			close(cli_fd);
			cli_rm(cli_fd);
			printf("Client No.%d disconnected!\n", cli_fd);
			break;
		}else{
			printf("Receive string from client No.%d:\n%s\n",cli_fd, buffer);
			strcpy(string[str_index], buffer);
			who[str_index] = cli_fd;
			str_index = (str_index+1)%STR_BUFFER;
		}
	}
	return 0;
}

void *client_send(void *argu){
	int cli_fd = *((int *)argu);
	char buffer[BUFFER_SIZE];
	int local_str_index = str_index-1;
	while(if_cli_con(cli_fd)){
		if(local_str_index != (str_index-1)){
			local_str_index = (local_str_index+1)%STR_BUFFER;
			if(who[local_str_index] != cli_fd){
				sprintf(buffer, "%d", who[local_str_index]);
				send(cli_fd, buffer, sizeof(buffer), 0);
				send(cli_fd, string[local_str_index], sizeof(string[local_str_index]), 0);
			}
		}
	}
	return 0;
}

int if_cli_con(int cli_fd){
	for(int i=0; i<THREAD_NUM; i++){
		if(connected_client[i] == cli_fd){
			return 1;
		}
	}
	return 0;
}

void cli_rm(int cli_fd){
	for(int i=0; i<THREAD_NUM; i++){
		if(connected_client[i] == cli_fd){	
			connected_client[i] = 0;
			break;
		}
	}
}
