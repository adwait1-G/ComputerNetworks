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
 *
 * ######NOTE: This program is broken. Unable to create 2 sockets together. Should check why this is happening. 
 */

#include "app.h"

int main(int argc, char **argv) {


	//Parsing Arguments. 
	if(argc != 5) {
		fprintf(stdout, "Usage: $ %s <YourName> <FriendName> <YourIpAddr> <FriendIpAddr>\n", argv[0]);
		exit(1);
	}
	

	
	MyName = argv[1];
	FriendName = argv[2];
	MyIpAddr = argv[3];
	FriendIpAddr = argv[4];

	//To give a feel of an application :P
	printf("\nConfiguring Application...\n");


	//Creating and Configuring Send socket. 
	//Create Socket
	send_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(send_fd == -1)
		err_exit("Error: Unable to create Sender Socket");

	//Populate sockaddr_in structure
	my_send_sock.sin_family = AF_INET;
	my_send_sock.sin_port = 5050;
	if((my_send_sock.sin_addr.s_addr = inet_addr(MyIpAddr)) == -1)
		err_exit("Error: Unable to convert YourIpAddr into NetworkOrderedIpAddr");
	memset(my_send_sock.sin_zero, '\0', 8);

	//Bind socket with sockaddr_in structure
	if(bind(send_fd, (struct sockaddr *)&my_send_sock, sizeof(struct sockaddr)) == -1)
		err_exit("Error: Unable to bind Sender Socket with Specified Tuple");


	//Creating and Configuring Receive socket.
	//Create socket.  
	recv_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(recv_fd != -1)
		err_exit("Error: Unable to create Receiver Socket");

	//Populate sockaddr_in structure
	my_recv_sock.sin_family = AF_INET;
	my_recv_sock.sin_port = 5051;
	if((my_recv_sock.sin_addr.s_addr = inet_addr(MyIpAddr)) == -1)
		err_exit("Error: Unable to convert YourIpAddr into NetworkOrderedIpAddr");
	memset(my_send_sock.sin_zero, '\0', 8);
	
	//Bind socket with sockaddr_in structure
	if(bind(recv_fd, (struct sockaddr *)&my_recv_sock, sizeof(struct sockaddr)) == -1)
		err_exit("Error: Unable to bind Receiver Socket with Specified Tuple");



	sleep(3);
	printf("Initialization and Configuration done.\n");

	fork_value = fork();

	if(fork_value == -1)
		err_exit("Error: Unable to fork a new process");
	
	//Child process will be receiver();
	else if(fork_value == 0)
		receiver(recv_fd);

	//Main process will be sender();
	else if(fork_value > 0)
		sender(send_fd);

	close(send_fd);
	close(recv_fd);

	return 0;
}
