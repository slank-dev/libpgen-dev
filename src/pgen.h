
#ifndef PGEN_H
#define PGEN_H

// include must!!
#include "pgen-macro.h"
#include "packet.h"
#include "pgen-opcode.h"
#include "pgen-funcs.h"
#include "pgen-variable.h"



#define PGEN_DEVLEN 256
#define PGEN_PACKLEN 2000
#define PGEN_ERROR -1
#define PGEN_SCCESS 1

#define PGEN_SEND_DEFAULT_PORT 7777
#define PGEN_DEFAULT_TTL 68	
	
#define PGEN_PACKETTYPE_ETH 	1
#define PGEN_PACKETTYPE_ARP 	2
#define PGEN_PACKETTYPE_IP		3
#define PGEN_PACKETTYPE_ICMP	4
#define PGEN_PACKETTYPE_TCP		5
#define PGEN_PACKETTYPE_UDP		6




#endif

