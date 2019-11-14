/*
 * This is simply an experiment to get started with "packet sockets".
 * This is helpful in writing protocols and drivers in the userspace and test our protocols in realtime. 

 * Written in C++
 */


#include<bits/stdc++.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<linux/if_ether.h>
#include<linux/if_packet.h>
#include<linux/if_arp.h>
#include<arpa/inet.h>
#include<netinet/ip.h>


using namespace std;

void err_exit(const char *errmsg) {

	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

int main() {

	int packet_socket;
	packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(packet_socket == -1)
		err_exit("Error in creating a packet socket");

	char buffer[5000];
	struct sockaddr src_addr;
	unsigned addrlen = sizeof(src_addr);
	
	unsigned long packet_count = 0;
	while(1)  {

		printf("packet_number = %lu\n", packet_count);

	int ret_recvfrom = recvfrom(packet_socket, buffer, sizeof(buffer), 0, &src_addr, (socklen_t *)&addrlen );
	
	if(ret_recvfrom < 0)
		err_exit("Error in receiving packets");

	printf("%s\n", buffer);

	struct ethhdr *EthHdr;
	struct iphdr *IpHdr;

	EthHdr = (struct ethhdr *)buffer;
	
	printf("SourceAddress = %02x:%02x:%02x:%02x:%02x:%02x\n", EthHdr->h_source[0], EthHdr->h_source[1], EthHdr->h_source[2], EthHdr->h_source[3], EthHdr->h_source[4], EthHdr->h_source[5]);
	printf("DestAddress = %02x:%02x:%02x:%02x:%02x:%02x\n", EthHdr->h_dest[0], EthHdr->h_dest[1], EthHdr->h_dest[2], EthHdr->h_dest[3], EthHdr->h_dest[4], EthHdr->h_dest[5]);
	

	printf("\n\n#############################\n");

	packet_count++;

	}

	return 0;
}