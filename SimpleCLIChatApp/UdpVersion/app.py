#!/usr/bin/python3

# * This is a Simple UDP-based ChatApp. 
# * Objectives:
# * 1. To get used to Socket Programming in Python
# * 2. To see how robust and secure an App I can make. 
# *
# * 
# * Specifications of the Application: 
# * 1. Transport Layer Protocol: UDP
# * 2. Uses IPv4
# * 3. This App uses 2 sockets. One to send messages and other to receive messages. 
# * 4. Port of Sender socket = 5050;
# * 5. Port of Receiver socket = 5051;
# * 6. Want to make it as robust as possible. 

# NOTE: This is still untested. Should have 2 machines to test it. 


import os
import socket
import sys


if __name__ == '__main__': 


	if len(sys.argv) != 5 :
		print("Usage: $ " + sys.argv[0] + " <YourName> <FriendName> <YourIpAddr> <FriendIpAddr>")
		sys.exit()
 
	MyName = sys.argv[1]
	FriendName = sys.argv[2]
	MyIpAddr = sys.argv[3]
	FriendIpAddr = sys.argv[4]

	print("Initializing and Configuring Application")

	SenderPort = 9090
	ReceiverPort = 9091
	
	MySenderTuple = (MyIpAddr, SenderPort)
	MyReceiverTuple = (MyIpAddr, ReceiverPort)
	FriendSenderTuple = (FriendIpAddr, SenderPort)
	FriendReceiverTuple = (FriendIpAddr, ReceiverPort)

	SendData = 0
	RecvData = 0
	ReceiverTuple = ()
	SentDataLength = 0
	RecvDataLength = 10000


	#Creating and Configuring SenderSocket. 
	SenderSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	SenderSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	SenderSocket.bind(MySenderTuple)

	#Creating and Configuring ReceiverSocket. 
	ReceiverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	ReceiverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	ReceiverSocket.bind(MyReceiverTuple)

	print("Done")

	fork_value = os.fork();

	if fork_value == 0 :
		
		while True :

			RecvData, ReceiverTuple = ReceiverSocket.recvfrom(RecvDataLength)
			
			if ReceiverTuple != FriendReceiverTuple :
				print("\nIntruder alert!!!")
				sys.exit()

			print("[" + str(FriendName) + "]>>> " + str(RecvData.decode('utf-8')), end='')

	elif fork_value > 0 :
		
		while True :

			print("[" + str(MyName) + "]>>> ", end='')
			SendData = str(input())
			SentDataLength = SenderSocket.sendto(SendData.encode('utf-8'), FriendReceiverTuple)




	socket.close(SenderSocket)
	socket.close(ReceiverSocket)
