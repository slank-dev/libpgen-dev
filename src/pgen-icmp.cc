
#include <map>
#include <string>
#include <iostream>

/* include must!!! */
#include "pgen-packet.h"
#include "pgen-variable.h"
#include "pgen-macro.h"
#include "pgen-opcode.h"
#include "pgen-funcs.h"
#include "util.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

//#include <net/ethernet.h> 
//#include <netinet/ip.h>
#include <netinet/ip_icmp.h>



pgen_icmp::pgen_icmp(){
	clear();
}




void pgen_icmp::info(){
	pgen_ip::info();

	std::map<int, const char*>  _icmpoption;
	_icmpoption[0] = "Echo Reply";
	_icmpoption[5] = "REdirect"; 
	_icmpoption[8] = "Echo"; 
	std::map<int, const char*> _icmpcode;
	_icmpcode[0]  = "Net Unreachable";
	_icmpcode[1]  = "Host Unreachable";
	_icmpcode[2]  = "Protocol Unreachable";
	_icmpcode[3]  = "Port Unreachable";
	_icmpcode[255]  = "Not Set";

	printf(" * Internet Control Message Protocol \n");
	printf("    - Type        :  %s (%d)\n", _icmpoption[icmp.type] , icmp.type);
	printf("    - Code        :  %s (%d)\n",  _icmpcode[icmp.code], icmp.code);
}


void pgen_icmp::clear(){
	pgen_packet::clear();
	icmp_option -1;
	icmp_code = -1;
}



void pgen_icmp::compile(const char* ifname){
	pgen_ip::compile(ifname);

	memset(data, 0, sizeof(data));
	memset(&icmp, 0, sizeof icmp);
		
	packetType = PGEN_PACKETTYPE_ICMP;
	ip.protocol = IPPROTO_ICMP;
	ip.tot_len = sizeof(ip) + sizeof(icmp);

	icmp.type = icmp_option;
	icmp.code = icmp_code;
//	icmp.check = checksum(&ip, sizeof(ip));
	icmp.checksum = checksum(&icmp, sizeof icmp);
	
	
	u_char* p = data;
	memcpy(p, &eth, sizeof(eth));
	p += sizeof(eth);
	memcpy(p, &ip, sizeof(ip));
	p += sizeof(ip);
	memcpy(p, &icmp, sizeof(icmp));
	len = p-data;
	
}



