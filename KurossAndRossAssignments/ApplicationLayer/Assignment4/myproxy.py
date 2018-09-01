#!/usr/bin/python3

# This is a simply Proxy Server written in python to understand the internals of a proxy. 
# It implements simple forwarding and redirecting back requests. 
# Doesn't implement caching of webpages
# Implements multiprocessing, so requests will be satisfied in parallel. 
# Cannot handle logins and other cookie-based http actions. 

import os
import sys
import time
import socket
import datetime
import multiprocessing as mp


def StartProxyService(NewConnection) :


	#Some variables required. 
	HttpRequestFromClient = ''
	Buffer = ''
	HttpRequestSize = 10000000
	ServerResponseMsgSize = 10000000
	RequestLine = ''
	crlf = "\r\n"


	#Receive the Client's Request
	TalkToClientSocket = NewConnection[0]
	ClientTuple = NewConnection[1]

	print("Process - " + str(os.getpid()) + " : Handling Client ", end='')
	print(ClientTuple)


	HttpRequestFromClient = TalkToClientSocket.recv(HttpRequestSize)	
	HttpRequestFromClient = HttpRequestFromClient.split(crlf.encode('utf-8'))


	##Parse the Client's Request - which will help in crafting appropriate HTTPRequest to the Server
	for HeaderPart in HttpRequestFromClient :
		HeaderPart = HeaderPart.decode('utf-8')
		print(HeaderPart)

	try :

		RequestLine = HttpRequestFromClient[0]
		RequestLine = RequestLine.split(' '.encode('utf-8'))
		HttpMethod = RequestLine[0].decode('utf-8')
		RequestedFile = RequestLine[1].decode('utf-8')
		HttpVersion = RequestLine[2].decode('utf-8')

	except UnicodeDecodeError :
		print("UnicodeDecodeError occured")


	#Crafting HTTPRequest to the Server
	HttpRequestToServer = ''
	ServerURL = ''
	ActualRequestedFile = ''
	SlashCount = 0
	Counter = 0

	while len(RequestedFile) != 0 :

		ch = RequestedFile[Counter]
		if(ch == '/' and SlashCount == 0) :
			SlashCount = 1


		elif (ch != '/' and SlashCount == 1) :
				ServerURL = ServerURL + str(ch)

		elif (ch == '/' and SlashCount == 1) :
			SlashCount = 2

		elif (SlashCount == 2) :
			ActualRequestedFile = ActualRequestedFile + str(ch)

		RequestedFile = RequestedFile[1:]

	ActualRequestedFile = "/" + ActualRequestedFile

	RequestLineToServer = HttpMethod + " " + ActualRequestedFile + " " + HttpVersion

	#Populate the HTTPRequest for the intended Server
	HttpRequestToServer = RequestLineToServer + " \r\n"
	HttpRequestToServer = HttpRequestToServer + "Host: " + ServerURL + " \r\n"
	
	Counter = 2

	while Counter < len(HttpRequestFromClient) :

		#if str(HttpRequestFromClient[Counter]).find("User-Agent") != -1  :
		#	HttpRequestToServer = HttpRequestToServer + "User-Agent: You don't know me" + "\r\n"

		if str(HttpRequestFromClient[Counter]).find("Cache-Control") != -1 :
			Counter = Counter + 1
			continue

		else:
			HttpRequestToServer = HttpRequestToServer + HttpRequestFromClient[Counter].decode('utf-8') + "\r\n"
		
		Counter = Counter + 1			

	print(HttpRequestToServer)
	##HTTpRequest for Server is ready. 

	##Connect to the Server and send the crafted HTTPRequest. 
	ServerIpAddr = socket.gethostbyname(ServerURL)
	ServerPortNo = 80
	ServerTuple = (ServerURL, ServerPortNo)

	TalkToServerSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	TalkToServerSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	TalkToServerSocket.connect(ServerTuple)

	TalkToServerSocket.send(HttpRequestToServer.encode('utf-8'))
	
	ServerResponseMsg = TalkToServerSocket.recv(ServerResponseMsgSize)

	#Send back the Server's reponse to Client
	SentBytesCount = TalkToClientSocket.send(ServerResponseMsg)
	
	ServerResponseMsg = ServerResponseMsg.split("\r\n".encode('utf-8'))
	
	for Part in ServerResponseMsg :
		Part = Part.decode('utf-8')
		print(Part)


	TalkToClientSocket.close()
	TalkToServerSocket.close()



if __name__ == '__main__':

	if len(sys.argv) != 4 :
		print("Usage: $ ./" + sys.argv[0] + " <IpAddress> <PortNumber> <ProxyCapacity> ");
		sys.exit()


	ProxyIpAddr = sys.argv[1]
	ProxyPortNo = int(sys.argv[2])
	ProxyTuple = (ProxyIpAddr, ProxyPortNo)
	ProxyCapacity = int(sys.argv[3])
	#Server Side of the Proxy is configured and  ready!
	ListenToClientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	ListenToClientSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	ListenToClientSocket.bind(ProxyTuple)
	ListenToClientSocket.listen(ProxyCapacity)

	print("ProxyLog: Proxy Listening on ", end='')
	print(ProxyTuple)

	ProcessList = []
	ProcessCount = 0

	while ProcessCount < ProxyCapacity :

		NewConnection = ListenToClientSocket.accept()
		HandleRequestProcess = mp.Process(target = StartProxyService, args = (NewConnection, ))
		ProcessList.append(HandleRequestProcess)
		print(ProcessList)
		HandleRequestProcess.start()
		ProcessCount = ProcessCount + 1


	for Process in ProcessList :
		Process.join()









