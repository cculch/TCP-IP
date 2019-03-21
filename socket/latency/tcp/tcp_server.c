#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define BUFFER_SIZE 100
#define SER_PORT 8003
#define SER_ADDR "127.0.0.1"

int lat_cal(long *ser_sec,long *ser_usec, long *cli_sec,long *cli_usec, long *diff_sec,long *diff_usec);

int main (int argc, char* argv[]){
	char buffer[BUFFER_SIZE];
	int sock_fd, client_sock_fd;
	struct sockaddr_in ser_info,cli_info;
	int addr_len = sizeof(cli_info);
	long ser_sec,ser_usec,cli_sec,cli_usec;
	struct timeval tv;
	struct timezone tz;
	char *tmp;
	long diff_sec,diff_usec;

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
		gettimeofday(&tv,&tz);
		tmp = strtok(buffer,"+");
		cli_sec = (long)atoi(tmp);
		tmp = strtok(NULL,"+");
		cli_usec = (long)atoi(tmp);
		printf("Client Time: %ld sec, %ld microsec\n",cli_sec,cli_usec);
		printf("Server Time: %ld sec, %ld microsec\n",tv.tv_sec,tv.tv_usec);

		lat_cal(&tv.tv_sec, &tv.tv_usec, &cli_sec, &cli_usec, &diff_sec, &diff_usec);

		printf("Latency: %ld seconds, %ld microseconds\n\n",diff_sec,diff_usec);

		strcpy(buffer,"Welcome!");
		send(client_sock_fd, buffer, sizeof(buffer), 0);
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
