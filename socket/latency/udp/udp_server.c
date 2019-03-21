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
#include <sys/time.h>

#define SER_PORT 8002
#define BUFFER_SIZE 100

int lat_cal(long *ser_sec,long *ser_usec, long *cli_sec,long *cli_usec, long *diff_sec,long *diff_usec);

int main(int argc, char* argv[]){
	struct sockaddr_in info;
	int sock_fd, byte_recv;
	char buffer[BUFFER_SIZE];
	int info_len = sizeof(info),recv_err=0;
	struct timeval tv;
	struct timezone tz;
	char *tmp;
	long cli_sec,cli_usec;
	long diff_sec,diff_usec;

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
		recv_err = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_info, &client_info_len);
		gettimeofday(&tv,&tz);
		if(recv_err < 0){			
			printf("Receive data fail!\n");
		}
		else{
			tmp = strtok(buffer,"+");
			cli_sec = (long)atoi(tmp);
			tmp = strtok(NULL,"+");
			cli_usec = (long)atoi(tmp);
			printf("Client Time: %ld seconds, %ld microseconds\n",cli_sec,cli_usec);
			printf("Server Time: %ld seconds, %ld microseconds\n",tv.tv_sec, tv.tv_usec);
			//printf("%s\n", buffer);

			lat_cal(&tv.tv_sec, &tv.tv_usec, &cli_sec, &cli_usec, &diff_sec, &diff_usec);

			printf("Latency: %ld seconds, %ld microseconds\n\n",diff_sec,diff_usec);

			strcpy(buffer,"Welcome!");
			if(sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_info, sizeof(client_info)) == -1){
				printf("Fail to reply!\n");
			}
		}
	}
	return 0;

}

int lat_cal(long *ser_sec, long *ser_usec, long *cli_sec, long *cli_usec, long *diff_sec, long *diff_usec){
	if(*cli_sec > *ser_sec){
		*diff_usec = *ser_usec - *cli_usec;
		*diff_sec = *ser_sec - *cli_sec;
		if(*diff_usec < 0){
			*diff_usec += 1000000;
			*diff_sec -= 1;
		}
		
	}else if(*cli_sec == *ser_sec){
		*diff_sec = 0;
		*diff_usec = *ser_usec - *cli_usec;
	}else{
		return -1;
	}	
	return 0;
}
