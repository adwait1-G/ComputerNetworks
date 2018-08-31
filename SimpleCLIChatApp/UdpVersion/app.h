/*
 * This is a Simple UDP-based ChatApp. 
 * Objectives:
 * 1. To get used to Socket Programming in C
 * 2. To see how robust and secure an App I can make. 
 *
 * 
 * Specifications of the Application: 
 * 1. Transport Layer Protocol: UDP
 * 2. Uses IPv4
 * 3. This App uses 2 sockets. One to send messages and other to receive messages. 
 * 4. Port of Sender socket = 5050;
 * 5. Port of Receiver socket = 5051;
 * 6. Want to make it as robust as possible. 
 */


//Constants: 

const unsigned int BUF_SIZE = 1000;


//Required Header Files. 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>


//Simple Error-Handling function. 
void err_exit(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}


//Initializing required variables.
int send_fd, recv_fd, fork_value;
struct sockaddr_in my_send_sock, my_recv_sock, friend_send_sock, friend_recv_sock;
char *send_msg;
char *recv_msg;
char *MyName, *FriendName, *MyIpAddr, *FriendIpAddr;


void sender() {
	
	send_msg = (char *)malloc(BUF_SIZE);
	unsigned send_msg_size;
	unsigned send_sock_len = sizeof(friend_recv_sock);

	while(1) {

		printf("\n[%s]>>> ", MyName);
		fgets(send_msg, sizeof(send_msg), stdin);

		send_msg_size = sendto(send_fd, send_msg, sizeof(send_msg), 0, (struct sockaddr *)&friend_recv_sock, send_sock_len);
		if(send_msg_size != strlen(send_msg))
			err_exit("Error: Unable to send message");
	}

}

void receiver(int recv_fd) {

	recv_msg = (char *)malloc(BUF_SIZE);
	unsigned recv_msg_size;
	unsigned recv_sock_len = sizeof(friend_send_sock);

	while(1) {

		recv_msg_size = recvfrom(recv_fd, recv_msg, sizeof(recv_msg), 0, (struct sockaddr *)&friend_send_sock, &recv_sock_len);
		if(recv_msg_size <= 0) 
			err_exit("Error: Unable to recieve message");

		printf("\n[%s]>>> %s", FriendName, recv_msg);
	}

	return;
}
