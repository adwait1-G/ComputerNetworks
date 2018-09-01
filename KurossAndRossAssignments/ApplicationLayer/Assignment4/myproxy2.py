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

crlf = "\r\n"
HttpRequestSize = 1000000
ServerReponseMsgSize = 1000000
ServerPortNo = 80

def StartProxyService(NewConnection) :

	while True :

		TalkToClientSocket = NewConnection[0]
		ClientTuple = NewConnection[1]
	
		#Receive the Client's Request
		HttpRequestFromClient = ReceiveClientRequest(TalkToClientSocket, ClientTuple)

		#1. Parse Client's Request
		#2. Craft Request to be sent to Server
		HttpRequestToServer, ServerURL = CraftServerHttpRequest(HttpRequestFromClient)


		#1. Connect to Server and get an active socket
		#2. Send the Crafted Request to Server
		TalkToServerSocket = SendServerHttpRequest(HttpRequestToServer, ServerURL)

		ServerHttpResponse = ReceiveServerResponse(TalkToServerSocket)

		SendResponseToClient(TalkToClientSocket, ServerHttpResponse)


	TalkToClientSocket.close()
	TalkToServerSocket.close()


def ReceiveClientRequest(TalkToClientSocket, ClientTuple) :

	HttpRequestFromClient = TalkToClientSocket.recv(HttpRequestSize)
	HttpRequestFromClient = HttpRequestFromClient.split(crlf.encode('utf-8'))

	for i in range(0, len(HttpRequestFromClient)) :
		HttpRequestFromClient[i] = HttpRequestFromClient[i].decode('utf-8')

	print("Client: ", ClientTuple)
	print("\n")
	for Line in HttpRequestFromClient :
		print(Line)

	return HttpRequestFromClient



def CraftServerHttpRequest(HttpRequestFromClient) :

	RequestLine = HttpRequestFromClient[0]
	RequestLine = RequestLine.split(" ")
	print("###########LEngth of RequestLine = ", len(RequestLine))
	print("###########RequestLine = ", RequestLine)
	
	try :
		HttpMethod = RequestLine[0]
		RequestedFile = RequestLine[1]
		HttpVersion = RequestLine[2]

	except IndexError :
		print("DBG: Should Check why RequestLine is ''")


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
	HttpRequestToServer = RequestLineToServer + " " + crlf
	HttpRequestToServer = HttpRequestToServer + "Host: " + ServerURL + " " + crlf
	
	Counter = 2

	while Counter < len(HttpRequestFromClient) :
		HttpRequestToServer = HttpRequestToServer + HttpRequestFromClient[Counter] + crlf
		Counter = Counter + 1	

	print(HttpRequestToServer)

	return HttpRequestToServer, ServerURL


def SendServerHttpRequest(HttpRequestToServer, ServerURL) :

	ServerTuple1 = (ServerURL, ServerPortNo)
	ServerIpAddr = socket.gethostbyname(ServerURL)
	ServerTuple2 = (ServerIpAddr, ServerPortNo)


	TalkToServerSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	TalkToServerSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	TalkToServerSocket.connect(ServerTuple2)

	TalkToServerSocket.send(HttpRequestToServer.encode('utf-8'))

	return TalkToServerSocket


def ReceiveServerResponse(TalkToServerSocket) :

	ServerHttpResponse = TalkToServerSocket.recv(ServerReponseMsgSize)
	print(ServerHttpResponse)
	return ServerHttpResponse

def SendResponseToClient(TalkToClientSocket, ServerHttpResponse) :

	TalkToClientSocket.send(ServerHttpResponse)




if __name__ == "__main__" :

	if len(sys.argv) != 4 :
		print("Usage: $ ./" + sys.argv[0] + " <IpAddress> <PortNumber> <ProxyCapacity> ")
		sys.exit()


	ProxyIpAddr = sys.argv[1]
	ProxyPortNo = int(sys.argv[2])
	ProxyTuple = (ProxyIpAddr, ProxyPortNo)

	ProxyCapacity = int(sys.argv[3])

	ListeningSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	ListeningSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	ListeningSocket.bind(ProxyTuple)
	ListeningSocket.listen(ProxyCapacity)

	print("ProxyServer: Listening on ", end='')
	print(ProxyTuple)

	ProcessList = []
	ProcessCount = 0

	while ProcessCount < ProxyCapacity :

		NewConnection = ListeningSocket.accept()
		RequestHandlingProcess = mp.Process(target = StartProxyService, args=(NewConnection, ))
		ProcessList.append(RequestHandlingProcess)
		RequestHandlingProcess.start()

		ProcessCount = ProcessCount + 1


	for Process in ProcessList :
		Process.join()






