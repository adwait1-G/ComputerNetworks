/*
 * CMP: CraftMyPacket
 * Inspired by SCAPY's power
 * Written in C++ to make it faster and more powerful!
 * Written to understand in and out of packets, network testing etc.,

 * main.cpp : Defines all functions declared in cmp.hh
 */

#include<bits/stdc++.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<net/ethernet.h>
#include<net/if_arp.h>
#include<unistd.h>

namespace cmp {

using namespace std;

/*
 * 1. All constants required by the tool. 
 */

//Error return constants

const int ALL_GOOD = 0;
const int END_OF_FILE = -1;
const int DOT_NOT_FOUND = -2;
const int PACKET_NAME_A_KEYWORD = -3;
const int NOT_AN_OPERATION = -4;
const int PACKET_NOT_PRESENT = -5;



//Some Important constants. 

const int DEFAULT_UDP_SRC_PORT = 1234;
const int DEFAULT_UDP_DST_PORT = 1234;
const unsigned DEFAULT_PACKET_SIZE = 1000; //Layer2 + 3 + 4


//Keywords!
//Should update these lists everytime a new function / feature is added.
vector<string> OperationsList{"create()", "destroy()", "print()"};
vector<string> L2ProtocolList{"Ether()"};
vector<string> L3ProtocolList{"IP(), ARP()"}; //IP =>IPv4
vector<string> L4ProtocolList{"UDP(), ICMP()"};


/*
 * 2. All classes and Data Structures required by the tool. 
 */





//Layer 2 Class
class L2 {

	public:
		int l2_type;

};


class Ethernet: public L2 {

	private: 
		string SourceMacAddress;
		string DestMacAddress;
		string Protocol;

	public: 

		struct ether_header EtherHeader;

		Ethernet() {

			ether_addr *EtherAddr;

			EtherAddr = ether_aton("00:00:00:00:00:00");
			memcpy(&EtherHeader.ether_shost, &EtherAddr->ether_addr_octet, 6);
			SourceMacAddress = "00:00:00:00:00:00";

			EtherAddr = ether_aton("ff:ff:ff:ff:ff:ff");
			memcpy(&EtherHeader.ether_dhost, &EtherAddr->ether_addr_octet, 6);
			DestMacAddress = "ff:ff:ff:ff:ff:ff";

			EtherHeader.ether_type = 0;	//0 means nothing!
			Protocol = "None";
		}

		void print() {

			cout<<"####Ethernet####"<<endl;
			cout<<"1. SourceMacAddress: "<<SourceMacAddress<<endl;
			cout<<"2. DestMacAddress: "<<DestMacAddress<<endl;
			cout<<"3. Protocol: "<<Protocol<<endl;
		}
};


//Layer 3 Class
class L3 {

	public: 

		int l3_type;
};


class IP: public L3{

	private:
		string Protocol;
		string SourceAddress, DestAddress;

	public:

		struct iphdr IpHdr;

		IP() {

			IpHdr.ihl = sizeof(IpHdr) / 4;
			IpHdr.version = 4;
			IpHdr.tos = 0;
			IpHdr.tot_len = 0;
			IpHdr.id = 0;
			IpHdr.frag_off = 0;
			IpHdr.ttl = 64;
			IpHdr.protocol = IPPROTO_UDP;
			IpHdr.check = 0;
			IpHdr.saddr = inet_addr("127.0.0.1");
			IpHdr.daddr = inet_addr("127.0.0.1");

			Protocol = "IPPROTO_UDP";
			SourceAddress = "127.0.0.1";
			DestAddress = "127.0.0.1";
			

			//IpHdr.check = CalculateChecksum(IpHdr);

		}

		void print() {

			cout<<"####Ipv4####"<<endl;
			cout<<"1. Header Length: "<<IpHdr.ihl<<endl;
			cout<<"2. Version Number: "<<IpHdr.version<<endl;
			cout<<"3. Type of Service: "<<IpHdr.tos<<endl;
			cout<<"4. Total Length: "<<IpHdr.tot_len<<endl;
			cout<<"5. Packet ID: "<<IpHdr.id<<endl;
			cout<<"6. Fragment Offset: "<<IpHdr.frag_off<<endl;
			cout<<"7. Time To Live: "<<IpHdr.ttl<<endl;
			cout<<"8. Protocol: "<<Protocol<<endl;
			cout<<"9. Checksum: "<<IpHdr.check<<endl;
			cout<<"10. SourceAddress = "<<SourceAddress<<endl;
			cout<<"11. DestAddress = "<<DestAddress<<endl;

			return;
		}
};


//Layer 4 Class
class L4 {

	public:
		int l4_type;
};


class UDP {

	private:
		 struct udphdr UdpHdr;

	public:

		struct udphdr UdpHdr;

		UDP() {

			UdpHdr.source = htons(DEFAULT_UDP_DST_PORT);
			UdpHdr.dest = htons(DEFAULT_UDP_DST_PORT);
			UdpHdr.len = htons(8);	//8 bytes by default
			UdpHdr.check = 0;	

			//UdpHdr.check = CalculateChecksum(UdpHdr);
		}

		void print() {

			cout<<"####UDP####"<<endl;
			cout<<"SourcePort: "<<ntohs(UdpHdr.source)<<endl;
			cout<<"DestPort: "<<ntohs(UdpHdr.dest)<<endl;
			cout<<"HeaderLength: "<<ntohs(UdpHdr.len)<<endl;
			cout<<"Checksum: "<<UdpHdr.check<<endl;
		}
};




//Class for a "packet" object

class PACKET {

	private:

		static unsigned TotalPacketCount;
		string Name;

		

	public:

		Ethernet *l2;
		IP *l3;
		UDP *l4;

		PACKET(string PacketName) {

			Name = PacketName;

			l2 = new Ethernet;
			l3 = new IP;
			l4 = new UDP;
		}

		~PACKET() {

			delete l2;
			delete l3;
			delete l4;
		}

};





unordered_map<string, PACKET* > NamePacketMap;
PACKET *NewPacket;


/*
 * 3. All functions required by the tool. 
 */


void Prologue() {

	printf("________  ____  ____  ________\n");
	printf("| _____|  | _ \\/ _ |  | ____ |\n");
	printf("| |       | |    | |  | |__| |\n");
	printf("| |       | |    | |  | _____|\n");
	printf("| |____   | |    | |  | |     \n");
	printf("|______|  |_|    |_|  |_|     \n");



	cout<<"\nWelcome to CraftMyPacket Version1.0!!!"<<endl;
	cout<<"CMP is an Interpreter"<<endl;
	cout<<"General operations on a packet:"<<endl;
	cout<<"1. Create a new packet: packet_name.create()"<<endl;
	cout<<"2. Destroy a packet: packet_name.destroy()"<<endl;
	cout<<"3. Print details of packet: packet_name.print()"<<endl;
	cout<<"4. Cutomize a packet: Look into the examples!"<<endl;

	return;
}


bool IsPacketNameKeyword(string& PacketName) {

	vector<string>::iterator iter;

	//1. 
	iter = OperationsList.begin();
	while(iter != OperationsList.end()) {
		if(PacketName == *iter)
			return true;

		iter++;
	}

	//2. 
	iter = L2ProtocolList.begin();
	while(iter != L2ProtocolList.end()) {
		if(PacketName == *iter)
			return true;

		iter++;
	}

	//3. 
	iter = L3ProtocolList.begin();
	while(iter != L3ProtocolList.end()) {
		if(PacketName == *iter)
			return true;

		iter++;
	}

	//4. 
	iter = L4ProtocolList.begin();
	while(iter != L4ProtocolList.end()) {
		if(PacketName == *iter)
			return true;

		iter++;
	}

	return false;
}


bool IsOperation(string& Operation) {

	vector<string>::iterator iter;

	//1. 
	iter = OperationsList.begin();
	while(iter != OperationsList.end()) {
		if(Operation == *iter)
			return true;

		iter++;
	}

	//2. 
	iter = L2ProtocolList.begin();
	while(iter != L2ProtocolList.end()) {
		if(Operation == *iter)
			return true;

		iter++;
	}

	//3. 
	iter = L3ProtocolList.begin();
	while(iter != L3ProtocolList.end()) {
		if(Operation == *iter)
			return true;

		iter++;
	}

	//4. 
	iter = L4ProtocolList.begin();
	while(iter != L4ProtocolList.end()) {
		if(Operation == *iter)
			return true;

		iter++;
	}

	return false;
}


bool PacketNotPresent(string& PacketName) {

	if(NamePacketMap.find(PacketName) == NamePacketMap.end())
		return true;

	else 
		return false;
}
 

void CreatePacket(string PacketName) {

	NewPacket = new PACKET(PacketName);
	NamePacketMap.insert(make_pair(PacketName, 	NewPacket));

}


void DestroyPacket(string PacketName) {

	PACKET *DeletePacket = NamePacketMap[PacketName];
	NamePacketMap.erase(PacketName);
	delete DeletePacket;
}

void PrintPacket(string PacketName) {

	PACKET *Packet = NamePacketMap[PacketName];
	
	cout<<"\n##########PacketName: \""<<PacketName<<"\"##########"<<endl;
	cout<<endl;
	Packet->l2->print();
	cout<<endl;
	Packet->l3->print();
	cout<<endl;
	Packet->l4->print();
	cout<<endl;

	return;
}


void SendPacket(string PacketName) {

	PACKET *Packet;
	Packet = NamePacketMap[PacketName];


}


int ParseCommand(string& cmd) {

	string PacketName, Operation;
	string::iterator iter;
	int dot_pos;

	if(cmd == "exit()")
		return END_OF_FILE;

	//If user simply pressed enter
	if(cmd.length() == 0)
		return ALL_GOOD;

	//Checking if '.' is present or not. 
	dot_pos = cmd.find('.', 0);

	if(dot_pos == string::npos)
		return DOT_NOT_FOUND;


	iter = cmd.begin();
	
	//Get the packet_name
	while(*iter != '.') {
		PacketName.push_back(*iter);
		iter++;
	}

	//Skip the '.'
	iter = iter + 1;

	//Get the operation to be performed on the packet. 
	while(iter != cmd.end()) {
		Operation.push_back(*iter);
		iter++;
	}

	if(IsPacketNameKeyword(PacketName) == true)
		return PACKET_NAME_A_KEYWORD;

	if(IsOperation(Operation) == false)
		return NOT_AN_OPERATION;

	//Handling create() requests. 
	if(PacketNotPresent(PacketName) == true && Operation != "create()")
		return PACKET_NOT_PRESENT;

	if(Operation == "create()")
		CreatePacket(PacketName);

	if(Operation == "destroy()")
		DestroyPacket(PacketName);

	if(Operation == "print()")
		PrintPacket(PacketName);

	if(Operation == "send()") 
		SendPacket(PacketName);
}


//Interpreter: Workhorse of the tool. 
void Interpreter() {


	string cmd;
	int pc_ret_value;

	cout<<"Starting Console..."<<endl;
	sleep(3);

	while(1) {

		cmd.clear();
		cout<<"CMD>>>";
		getline(cin, cmd);

		pc_ret_value = ParseCommand(cmd);

		if(pc_ret_value == END_OF_FILE)
			return;

		//If Returned simply
		if(pc_ret_value == ALL_GOOD)
			continue;

		//If Dot not found. 
		if(pc_ret_value == DOT_NOT_FOUND) 
			cout<<"SyntaxError in the command entered"<<endl;

		if(pc_ret_value == PACKET_NAME_A_KEYWORD)
			cout<<"NamingError: Packet name cannot be a Keyword!"<<endl;

		if(pc_ret_value == NOT_AN_OPERATION)
			cout<<"OperationError: Such an operation does not exist"<<endl;

		if(pc_ret_value == PACKET_NOT_PRESENT)
			cout<<"PacketNotFoundError: This packet is not present"<<endl;

	}


	return;
}







}