#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define SER_PORT 8002
#define SER_ADDR "127.0.0.1"
#define BUFFER_SIZE 100



int main(int argc, char* argv[]){
	struct sockaddr_in info;
	int sock_fd, info_size;
	char buffer[BUFFER_SIZE];
	struct timeval tv;
	struct timezone tz;
	

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == -1){
		printf("Fail to create a socket!\n");
	}

	bzero(&info, sizeof(info));
	info.sin_family = AF_INET;
	info.sin_port = htons(SER_PORT);
	info.sin_addr.s_addr = inet_addr(SER_ADDR);


	gettimeofday(&tv,&tz);
	printf("tv_sec: %ld\ntv:usec: %ld\n",tv.tv_sec,tv.tv_usec);
	sprintf(buffer,"%ld+%ld",tv.tv_sec,tv.tv_usec);
	if(sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&info, sizeof(info)) == -1){
		printf("Fail to send a packet!\n");
	}
	info_size = sizeof(info);
	bzero(buffer,BUFFER_SIZE);
	if(recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&info, &info_size) < 0){
		printf("Fail to seceive from server!\n");
	}else{
		printf("Receive from server:\n");
		printf("%s\n",buffer);
	}

	close(sock_fd);
	return 0;
}


