#!/usr/bin/python3

#Assignment 1: Web Server
#In this assignment, you will develop a simple Web server in Python that is capable
#of processing only one request. Specifically, your Web server will 
#(i) create a connection socket when contacted by a client (browser); 
#(ii) receive the HTTP request from this connection; 
#(iii) parse the request to determine the specific file being requested; 
#(iv) get the requested file from the server’s file system; 
#(v) create an HTTP response message consisting of the requested file preceded by header lines;
#(vi) send the response over the TCP connection to the requesting browser.
#If a browser requests a file that is not present in your server, 
#your server should return a “404 Not Found” error message.

# This python script correctly answers the Assignment1. 
# This is a very basic server. 
# This won't even check if the protocol being used is HTTP in the HTTPRequest. 

#Had a lot of fun writing it!!

import os
import sys
import socket
import time
import datetime


if __name__ == '__main__': 

	if len(sys.argv) != 3 : 
		print("Usage: $ python3" + sys.argv[0] + " <IpAddress> <PortNumber>")
		sys.exit()


	ServerIpAddr = sys.argv[1]
	ServerPortNo = int(sys.argv[2])
	ServerTuple = (ServerIpAddr, ServerPortNo)

	HttpRequest = ''
	HttpRequestSize = 10000
	ClientTuple = ()
	RequestLine = ''
	
	##Creating and Configuring the ReceiveSocket.
	ReceiveSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	ReceiveSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	ReceiveSocket.bind(ServerTuple)
	ReceiveSocket.listen(1)

	print("ServerLog: HTTPServer is listening on ", end='')
	print(ServerTuple)

	NewConnection = ReceiveSocket.accept()
	ClientSocket = NewConnection[0]
	ClientTuple = NewConnection[1]

	HttpRequest = ClientSocket.recv(HttpRequestSize)

	print("This is the Client: ", end='')
	print(ClientTuple)
	print("\nHttpRequest: \n")
	
	crlf = "\r\n"

	HttpRequest = HttpRequest.split(crlf.encode('utf-8'))

	for HeaderPart in HttpRequest :
		HeaderPart = HeaderPart.decode('utf-8')
		print(HeaderPart)

	RequestLine = HttpRequest[0]
	RequestLine = RequestLine.split(' '.encode('utf-8'))
	HttpMethod = RequestLine[0].decode('utf-8')
	RequestedFile = RequestLine[1].decode('utf-8')
	HttpVersion = RequestLine[2].decode('utf-8')


	File = ''

	try:
		FileObject = open(RequestedFile)
		File = FileObject.read()
		ResponseLine = HttpVersion + " 200 OK\r\n"

	except FileNotFoundError: 
		FileObject = open("Error404File")
		File = FileObject.read()
		print("ServerLog: File Not Found. Returning Error404")
		ResponseLine = HttpVersion + " 404 Not Found\r\n"


	# Populating HttpResponseHeader
	HttpResponseHeader = ResponseLine
	HttpResponseHeader = HttpResponseHeader + "Date:" + str(time.ctime(datetime.datetime.now().timestamp())) + "\r\n"
	HttpResponseHeader = HttpResponseHeader + "Server: You can't catch me\r\n"
	HttpResponseHeader = HttpResponseHeader + "X-Powered-By: My own Scripting Language\r\n"
	HttpResponseHeader = HttpResponseHeader + "Expires: When you expire\r\n"
	HttpResponseHeader = HttpResponseHeader + "Cache-Control: Out of Control\r\n"
	HttpResponseHeader = HttpResponseHeader + "Link: youknowwho.hackme\r\n"
	HttpResponseHeader = HttpResponseHeader + "Content-Type: text/html; charset=UTF-8\r\n"
	HttpResponseHeader = HttpResponseHeader + "\r\n"

	HttpResponsePacket = HttpResponseHeader + File 


	ClientSocket.send(HttpResponsePacket.encode('utf-8'))

	ClientSocket.close()
	ReceiveSocket.close()














