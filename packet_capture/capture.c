#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<netinet/in.h>

#include<unistd.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<linux/if_ether.h>
#include<linux/ip.h>

#define NET_INTERFACE "enx503eaabb51e7"
#define BUFFER_SIZE 1600

struct ether_header{
	unsigned char ether_dhost[ETH_ALEN];
	unsigned char ether_shost[ETH_ALEN];
	unsigned short ether_type;
};

int createSock();//need root
void resultPri(int ip, int arp, int rarp, int tcp, int udp, int icmp, int igmp);

int main(int argc, char* argv[]){
	unsigned char buffer[ETH_FRAME_LEN];
	int sock_fd;
	struct ether_header *peth;
	struct iphdr *pip;
	char *ptemp;
	struct ifreq ifr;
	struct sockaddr_in recv_addr;
	int recv_len;
	int addr_len = sizeof(recv_addr);
	int ip=0, arp=0, rarp=0, tcp=0, udp=0, icmp=0, igmp=0;


	strncpy(ifr.ifr_name, NET_INTERFACE, sizeof(NET_INTERFACE)+1);
	sock_fd = createSock();
	if(ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1){
		perror("ioctl");
		exit(1);
	}
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1){
		perror("ioctl");
		exit(3);
	}

	for(int i=0; i<100; i++){
		recv_len = recvfrom(sock_fd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
		//ethernet count
		ptemp = buffer;
		peth = (struct ether_header *)ptemp;//ethernet header
		switch(ntohs(peth->ether_type)){
			case 0x0800:
				ip++;
				break;
			case 0x0806:
				arp++;
				break;
			case 0x8035:
				rarp++;
				break;
		}
		//ip count
		ptemp += sizeof(struct ether_header);
		pip = (struct iphdr *)ptemp;
		switch(pip->protocol){
			case IPPROTO_TCP:
				tcp++;
				break;
			case IPPROTO_UDP:
				udp++;
				break;
			case IPPROTO_ICMP:
				icmp++;
				break;
			case IPPROTO_IGMP:
				igmp++;
				break;
		}
	}
	resultPri(ip, arp, rarp, tcp, udp, icmp, igmp);

	return 0;
}

int createSock(){
	int sock_fd;
	sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_fd < 0){
		printf("Raw Socket: Creation Failed!\n");
		exit(1);
	}
	printf("Raw Socket: Creation Succeeded!\n");
	return sock_fd;
}

void resultPri(int ip, int arp, int rarp, int tcp, int udp, int icmp, int igmp){
	printf("------statistics------\n");
	printf("%-10s:%d\n", "IP", ip);
	printf("%-10s:%d\n", "ARP", arp);
	printf("%-10s:%d\n", "RARP", rarp);
	printf("%-10s:%d\n", "TCP", tcp);
	printf("%-10s:%d\n", "UDP", udp);
	printf("%-10s:%d\n", "ICMP", icmp);
	printf("%-10s:%d\n", "IGMP", igmp);
	printf("------finish------\n");
}
