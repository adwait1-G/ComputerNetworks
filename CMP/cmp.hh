/*
 * CMP: CraftMyPacket
 * Inspired by SCAPY's power
 * Written in C++ to make it faster and more powerful!
 * Written to understand in and out of packets, network testing etc.,

 * cmp.hh : Has all Data Structures and Function Declarataions required. 
 */


#ifndef _CMP_HH
#define _CMP_HH 1

#include<bits/stdc++.h>

namespace cmp {
using namespace std;



//prologue() prints logo and explains what CMP is. 
void Prologue();


//High Level function which takes of all the surface-level operations.
//1. Running the interpreter loop
//2. Making sure proper parsing of the command is done. 
void Interpreter();


//Checks if packet_name is a Keyword. 
bool IsPacketNameKeyword(string& PacketName);


//Checks if the operation identified is an actual operation.
bool IsOperation(string& Operation);


//Routine that parses the command entered and decides what to do, which object to create, destroy etc., or should raise an error. 
void ParseCommand(string& cmd);


void Create(string& PacketName);













}


#endif 