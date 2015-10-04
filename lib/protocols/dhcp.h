#ifndef DHCP_H
#define DHCP_H



#include "udp.h"



#define DHCP_MAX_OPT 64
class pgen_dhcp : public pgen_udp {
	protected:
		struct dhcp_header dhcp;
		bit8  dhcp_option[256];
		bit32 dhcp_option_len;
	public:
		static const int minLen = pgen_udp::minLen+DNS_HDR_LEN;
		static const int maxLen = PGEN_MAX_PACKET_LEN; 
		struct{
			bit8   op;
			bit8   htype;
			bit8   hlen;
			bit8    hops;
			bit32   xid;
			bit16   secs;
			bit16   flags;
			ipaddr  ciaddr;
			ipaddr  yiaddr;
			ipaddr  siaddr;
			ipaddr  giaddr;
			macaddr chaddr;
			bit8    sname[64];
			bit8    file[128];
			
			bit32   option_len;
			struct dhcp_option option[DHCP_MAX_OPT];
		}DHCP;
		
		pgen_dhcp();
		pgen_dhcp(const void*, int);
		void clear();
		void compile();
		void cast(const void*, int);
		void send(const char* ifname){send_L3(ifname);}
		void summary();
		void info();
		void help();

		void dhcp_set_option(int,int,int,void*);
		void dhcp_get_option(const void*, struct dhcp_option*);
};



#endif
