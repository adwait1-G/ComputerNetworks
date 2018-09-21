/*
 * This is an implementation of a ftp-like client. 
 * It has to be used with myftp_server.c to experiment and play with. 
 * This doesn't implement the RFC. 
 * 
 * MIT License: 
 * 
 * Copyright 2018 Adwaith Gautham
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the 
 * Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH 
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * 
 * Basically, anyone can use it. Please use it, learn from it and have fun!
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

void StartControlCommandLine(int ControlConFd);

void err_exit(const char *errmsg) {
    printf("%s\n", errmsg);
    exit(-1);
}


int main(int argc, char **argv) {

    if(argc != 5) {
        fprintf(stderr, "Usage: $ %s <MyFTPClientIpAddress> <ClientControlPort> <MyFTPServerIpAddress> <ServerControlPort> \n", argv[0]);
        exit(1);
    }
    
    int ControlConFd, DataConFd;
    struct sockaddr_in ServerControlStruct, ClientControlStruct, ServerDataStruct, ClientDataStruct;
    char *ClientIpAddress, *ServerIpAddress;
    unsigned short int ClientControlPort, ServerControlPort;

    /*
     * Update all the above variables. 
     * They will be used everywhere in the program. 
     */

    //Updating Client's Control Details. 
    ClientIpAddress = argv[1];
    ClientControlPort = atoi(argv[2]);
    ClientControlStruct.sin_addr.s_addr = inet_addr(ClientIpAddress);
    ClientControlStruct.sin_family = AF_INET;
    ClientControlStruct.sin_port = htons(ClientControlPort);
    memset(ClientControlStruct.sin_zero, '\0', sizeof(ClientControlStruct.sin_zero));

    //Updating Server's Control Details. 

    ServerIpAddress = argv[3];
    ServerControlPort = atoi(argv[4]);
    ServerControlStruct.sin_addr.s_addr = inet_addr(ServerIpAddress);
    ServerControlStruct.sin_family = AF_INET;
    ServerControlStruct.sin_port = htons(ServerControlPort);
    memset(ServerControlStruct.sin_zero, '\0', sizeof(ServerControlStruct.sin_zero));

    /*
     * Create ClientControl socket. 
     */

    ControlConFd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    if(ControlConFd == -1) {
        err_exit("Error in creating Client Control Socket");
    }

    if(bind(ControlConFd, (struct sockaddr *)&ClientControlStruct, sizeof(ClientControlStruct)) == -1)
        err_exit("Error in binding to the given (IPAddress, Port) Tuple");

    /*
     * Connect to the MyFTPServer using ServerControlStruct. 
     */

    if(connect(ControlConFd, (struct sockaddr *)&ServerControlStruct, sizeof(ServerControlStruct)) == -1)
        err_exit("Error in connecting with MyFTPServer");
    
    /*
     * Client's Control Connection gets a command line offered by the server. 
     * Client can use the command line and select which file to download. 
     */

    StartControlCommandLine(ControlConFd);

    if(close(ControlConFd) == -1)
        err_exit("Error closing Client Control Socket");
    
    return 0;
}


void StartControlCommandLine(int ControlConFd) {

    char CommandBuf[1000];
    char RecvBuf[10000];
    unsigned SendDataLength, RecvDataLength;

    while(1) {
        
        write(1, "\n>>> ", 5);
        fflush(stdout);
        
        //Command Send part!
        fflush(stdin);
        memset(CommandBuf, '\0', sizeof(CommandBuf));
        read(0, (void *)CommandBuf, sizeof(CommandBuf));
        
        SendDataLength = send(ControlConFd, (void *)CommandBuf, strlen(CommandBuf), 0);
        if(SendDataLength < strlen(CommandBuf))
            err_exit("Error in sending Control Command to Server");


        //Command Recv part!
        memset(RecvBuf, '\0', sizeof(RecvBuf));
        RecvDataLength = recv(ControlConFd, (void *)RecvBuf, sizeof(RecvBuf), 0);
        if(RecvDataLength == -1)
            err_exit("Error in receiving Control Data back from Server");
        
        printf("%d\n", RecvDataLength);
        printf("%s", RecvBuf);
    }

    return;
}    