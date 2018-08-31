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

# This python script is next version of HTTPServer.py
# A few improvements, which will make the server more usable. 
# You can request as many files as you want. 
# This server can queue 25 requests. But there is only 1 active connection at a time. 
# So, This http Server can cater to any browser's connection.  

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
	ServerSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	ServerSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	ServerSocket.bind(ServerTuple)
	ServerSocket.listen(25)

	print("ServerLog: HTTPServer is listening on ", end='')
	print(ServerTuple)

	while True:

		NewConnection = ServerSocket.accept()
		TalkToClientSocket = NewConnection[0]
		ClientTuple = NewConnection[1]

		HttpRequest = TalkToClientSocket.recv(HttpRequestSize)

		print("This is the Client: ", end='')
		print(ClientTuple)
		print("\nHttpRequest: \n")
	
		crlf = "\r\n"

		HttpRequest = HttpRequest.split(crlf.encode('utf-8'))

		for HeaderPart in HttpRequest :
			HeaderPart = HeaderPart.decode('utf-8')
			print(HeaderPart)


		try:

			RequestLine = HttpRequest[0]
			print(RequestLine)
			RequestLine = RequestLine.split(' '.encode('utf-8'))
			HttpMethod = RequestLine[0].decode('utf-8')
			RequestedFile = RequestLine[1].decode('utf-8')
			HttpVersion = RequestLine[2].decode('utf-8')
		except UnicodeDecodeError:
			print("UnicodeDecodeError occured.")


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


		TalkToClientSocket.send(HttpResponsePacket.encode('utf-8'))

		TalkToClientSocket.close()

	ServerSocket.close()














