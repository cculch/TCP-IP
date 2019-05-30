#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/ip_icmp.h>
#include<arpa/inet.h>

#define BUFFER_SIZE 1024

unsigned short checksum(unsigned short *buf, int bufsize);

int main(int argc, char* argv[]){
	int sock_fd;
	struct icmp icmp_hdr;
	struct icmp *recv_icmp_hdr;
	struct iphdr *recv_ip_hdr;
	struct sockaddr_in addr;
	char buffer[BUFFER_SIZE];
	struct timeval tv;
	struct timeval recv_tv;

	if(argc != 2){
		printf("Please input one target address!\n");
		exit(1);
	}

	addr.sin_family = PF_INET; //ipv4
	if(inet_pton(PF_INET, argv[1], &addr.sin_addr) < 0){
		printf("Fail to translate address!\n");
		exit(1);
	}
	//addr.sin_addr = inet_addr(argv[1]);

	sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sock_fd < 0){
		printf("Fail to create socket!\n");
		exit(1);
	}

	memset(&icmp_hdr, 0, sizeof(icmp_hdr));

	icmp_hdr.icmp_type = ICMP_TIMESTAMP;
	icmp_hdr.icmp_code = 0;
	icmp_hdr.icmp_cksum = 0;
	icmp_hdr.icmp_hun.ih_idseq.icd_id = 0;
	icmp_hdr.icmp_hun.ih_idseq.icd_seq = 0;
	gettimeofday(&tv, NULL);
	icmp_hdr.icmp_otime = (tv.tv_sec*1000000) + tv.tv_usec;
	printf("icmp_type = %d, icmp_code = %d\n", ICMP_TIMESTAMP, 0);
	printf("Original timestamp =  %u\n", icmp_hdr.icmp_otime);
	printf("Receive timestamp = %u\n", 0);
	printf("Transimit timestamp = %u\n", 0);
	printf("Final timestamp = %u\n", 0);
	icmp_hdr.icmp_cksum = checksum((unsigned short *)&icmp_hdr, sizeof(icmp_hdr));

	if(sendto(sock_fd, (char*)&icmp_hdr, sizeof(icmp_hdr), 0, (struct sockaddr*)&addr, sizeof(addr)) < 1){
		printf("Fail to send icmp request!\n");
		exit(1);
	}
	//printf("Send icmp successfully!\n");

	if(recv(sock_fd, buffer, sizeof(buffer), 0) < 1){
		printf("Fail to receive icmp reply!\n");
	}
	gettimeofday(&recv_tv, NULL);

	recv_ip_hdr = (struct iphdr*)buffer;
	recv_icmp_hdr = (struct icmp*)(buffer + ((recv_ip_hdr->ihl)<<2));

	//printf("Sended: %ld\n", (tv.tv_sec*1000000)+tv.tv_usec);
	//printf("Sended: %u\n", (unsigned int)((tv.tv_sec*1000000)+tv.tv_usec));
	printf("-------------------------------------\n");
	printf("icmp_type = %u, icmp_code = %u\n", recv_icmp_hdr->icmp_type, recv_icmp_hdr->icmp_code);
	printf("Original timestamp = %u\n", recv_icmp_hdr->icmp_otime);
	printf("Receive timestamp = %u\n", recv_icmp_hdr->icmp_rtime);
	printf("Transimit timestamp = %u\n", recv_icmp_hdr->icmp_ttime);
	printf("Final timestamp = %u\n", (unsigned int)((recv_tv.tv_sec*1000000)+recv_tv.tv_usec));
	printf("RTT = %u\n",((unsigned int)((recv_tv.tv_sec*1000000)+recv_tv.tv_usec) - recv_icmp_hdr->icmp_otime) - (recv_icmp_hdr->icmp_ttime - recv_icmp_hdr->icmp_rtime));
	return 0;
}

unsigned short checksum(unsigned short *buf, int bufsize){
	unsigned long sum = 0xffff;

	while (bufsize > 1){
		sum += *buf;
		buf++;
		bufsize -=2;
	}

	if(bufsize == 1){
		sum += *(unsigned char*)buf;
	}

	sum = (sum & 0xffff) + (sum >> 16);
	sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}
