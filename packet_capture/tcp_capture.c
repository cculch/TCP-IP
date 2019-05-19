#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<linux/if_ether.h>
//#include<linux/ip.h>

#define NET_INTERFACE "enx503eaabb51e7"
#define BUFFER_SIZE 1600

struct ether_header{
	unsigned char ether_dhost[ETH_ALEN];
	unsigned char ether_shost[ETH_ALEN];
	unsigned short ether_type;
};

int createSock();//need root

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
	int count = 0;
	unsigned char *own_mac;
	unsigned short temp, source_mac[6], destination_mac[6];
	unsigned int source_ip, destination_ip, mask = 255;
	struct tcphdr *tcp;


	strncpy(ifr.ifr_name, NET_INTERFACE, sizeof(NET_INTERFACE)+1);
	sock_fd = createSock();
	ioctl(sock_fd, SIOCGIFHWADDR, &ifr);
	own_mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
	printf("My own Mac: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", own_mac[0], own_mac[1], own_mac[2], own_mac[3], own_mac[4], own_mac[5]);
	if(ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == -1){
		perror("ioctl");
		exit(1);
	}
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(sock_fd, SIOCSIFFLAGS, &ifr) == -1){
		perror("ioctl");
		exit(3);
	}

	while(count < 10){
		recv_len = recvfrom(sock_fd, (char *)buffer, sizeof(buffer), 0, (struct sockaddr *)&recv_addr, &addr_len);
		//ethernet count
		ptemp = buffer;
		peth = (struct ether_header *)ptemp;//ethernet header
		if(ntohs(peth->ether_type) == 0x0800){
			//get mac address(source and destination)
			for(int i=0; i<6; i++){
				source_mac[i] = (unsigned short)peth->ether_shost[i];
				source_mac[i] <<= 8;
				source_mac[i] = ntohs(source_mac[i]);
				destination_mac[i] = (unsigned short)peth->ether_dhost[i];
				destination_mac[i] <<=8;
				destination_mac[i] = ntohs(destination_mac[i]);
			}
			//need to check if source and destination does not belong to me
			/***
			if((own_mac[0] == destination_mac[0])&&(own_mac[1] == destination_mac[1])){
				continue;
			}else if((own_mac[0] == source_mac[0])&&(own_mac[1] == source_mac[1])){
			continue;
			}***/
			//get ip protocol
			ptemp += sizeof(struct ether_header);
			//tcp = (struct tcphdr *)(ptemp + (ptemp->ip_hl << 2));
			pip = (struct iphdr *)ptemp;
			tcp = (struct tcphdr *)(ptemp + ((pip->ihl)<<2));
			if(pip->protocol != IPPROTO_TCP){
				continue;
			}

			printf("Source MAC address: %X:%X:%X:%X:%X:%X\n", source_mac[0], source_mac[1], source_mac[2], source_mac[3], source_mac[4], source_mac[5]);
			printf("Destination MAC address: %X:%X:%X:%X:%X:%X\n", destination_mac[0], destination_mac[1], destination_mac[2], destination_mac[3], destination_mac[4], destination_mac[5]);
			printf("IP->protocol = TCP\n");
			printf("IP->src_ip = %d.%d.%d.%d\n", (pip->saddr)&mask, ((pip->saddr)>>8)&mask, ((pip->saddr)>>16)&mask, (pip->saddr)>>24);
			printf("IP->dst_ip = %d.%d.%d.%d\n", (pip->daddr)&mask, ((pip->daddr)>>8)&mask, ((pip->daddr)>>16)&mask, (pip->daddr)>>24);
			printf("Src_port =%d\n", ntohs(tcp->th_sport));
			printf("Dst_port =%d\n", ntohs(tcp->th_dport));
		}else{
			continue;
		}
		printf("\n");
		count++;
	}

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
