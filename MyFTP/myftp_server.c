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
 * Basically, anyone can use it. Please use it, learn from it and have fun!
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

void HandleControlConnection(int ControlConFd, struct sockaddr_in **ClientControlStruct, int ClientNumber);

void err_exit(const char *errmsg) {
    printf("%s\n", errmsg);
    exit(-1);
}

int main(int argc, char **argv) {

    if(argc != 3) {
        fprintf(stderr, "Usage: $ %s <MyFTPServerIpAddress> <ServerControlPort> \n", argv[0]);
        exit(1);
    }

    const int ListenQueueSize = 20;

    int ServerListenFd, ControlConFd, ClientNumber = 0;
    unsigned int **ClientControlStructLen;
    struct sockaddr_in ServerListenStruct, **ClientControlStruct;
    char *ServerIpAddress;
    unsigned short int ServerControlPort;

    //Update the details. 
    ServerIpAddress = argv[1];
    ServerControlPort = atoi(argv[2]);

    ServerListenStruct.sin_addr.s_addr = inet_addr(ServerIpAddress);
    ServerListenStruct.sin_family = AF_INET;
    ServerListenStruct.sin_port = htons(ServerControlPort);
    memset(ServerListenStruct.sin_zero, '\0', sizeof(ServerListenStruct.sin_zero));

    ClientControlStruct = (struct sockaddr_in **)malloc(sizeof(struct sockaddr_in *) * ListenQueueSize);
    ClientControlStructLen = (unsigned int **)malloc(sizeof(unsigned int) * ListenQueueSize);
    //Allocate memory for maximum of ListenQueueSize number of connections. 
    for(int i = 0; i < ListenQueueSize; i++) {
        ClientControlStruct[i] = (struct sockaddr_in *)malloc(sizeof(struct sockaddr));
        ClientControlStructLen[i] = (unsigned int *)malloc(sizeof(unsigned int));
    }

    printf("ServerLog>> Memory Allocated, Variables Populated. Ready....\n");

    //Create a Listening socket!
    ServerListenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(ServerListenFd == -1)
        err_exit("Error in creating Listening socket");
    
    printf("ServerLog>> Created ServerListenFd\n");
    
    //Bind the Listening socket with ServerListenStruct structure's details. 
    if(bind(ServerListenFd, (struct sockaddr *)&ServerListenStruct, sizeof(ServerListenStruct)) == -1)
        err_exit("Error in binding the socket with specified details");

    //TODO: Should write this setsockopt() function.  
    //setsockopt(ServerListenFd, SOL_SOCKET, SO_REUSEADDR, 

    printf("ServerLog>> ServerListenFd successfully bound with ServerListenStruct details\n");


    //Listen to incoming connections. 
    if(listen(ServerListenFd, ListenQueueSize) == -1)
        err_exit("Error in listening to incoming connections");
    
    printf("ServerLog>> Listening at (%s, %u)\n", ServerIpAddress, ServerControlPort);

    //Will loop only till the number of clients reaches ListenQueueSize. After ListenQueueSize is hit, loop terminates. 
    while(ClientNumber < ListenQueueSize) {
        ControlConFd = accept(ServerListenFd, (struct sockaddr *)ClientControlStruct[ClientNumber], ClientControlStructLen[ClientNumber]);
        if(ControlConFd == -1)
            err_exit("Error in creating a new socket for an incoming Control connection");
        
        HandleControlConnection(ControlConFd, ClientControlStruct, ClientNumber);

        ClientNumber++;
    }

    return 0;
}

void HandleControlConnection(int ControlConFd, struct sockaddr_in **ClientControlStruct, int ClientNumber) {

    printf("ServerLog>> Handling Connection from (%s, %u)\n", inet_ntoa(ClientControlStruct[ClientNumber]->sin_addr), ClientControlStruct[ClientNumber]->sin_port);

    char SendBuf[10000], RecvBuf[10000], Command[10000], FileName[30];
    unsigned SendLength, RecvLength;
    FILE *CmdPointer;

    while(1) {
        
        //Receive the commands. 
        memset(RecvBuf, '\0', sizeof(RecvBuf));
        RecvLength = recv(ControlConFd, (void *)RecvBuf, sizeof(RecvBuf), 0 );
        if(RecvLength == -1) {
            fprintf(stderr, "Error in receiving Control Data from Client %d\n", ClientNumber);
            //exit(-1);
            return;     //If we exit, the whole server would be killed. Will spawn a new process for every client - Easy!
        }
        else if(RecvLength == 1 && strcmp(RecvBuf, "\n") == 0) {
            strcpy(SendBuf, "\n");
        }
        //If Client sends Ctrl+C or Ctrl+D, gracefully end the CommandLine
        else if(RecvBuf[0] == 03 || RecvBuf[0] == 04)
            return;
        else {
            //recv() appends a linefeed!
            RecvBuf[strlen(RecvBuf) - 1] = '\0';
        
            //Process the Commands and get the output. 
            snprintf(FileName,sizeof(FileName), "CommandOutput%d.txt", ClientNumber);
            snprintf(Command, sizeof(Command), "%s > CommandOutput%d.txt", RecvBuf, ClientNumber);
            
            system(Command);

            FILE *fp;
            fp = fopen(FileName, "r");
            int count = 0;
            while(1) {
                char ch = fgetc(fp);
                if(ch == EOF)
                    break;
                
                SendBuf[count] = ch;
                count++;
            }

            fclose(fp);
        }

        //Send the output.
        SendLength = send(ControlConFd, (void *)SendBuf, strlen(SendBuf), 0);
        if(SendLength == -1) {
            fprintf(stderr, "Error sending Response to Control Data of Client %d\n", ClientNumber);
            return;
        }
        memset(SendBuf, '\0', sizeof(SendBuf));
        
    }
    return;
}



















