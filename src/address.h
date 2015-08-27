#ifndef ADDRESS_H
#define ADDRESS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include "mytypes.h"

#define FILEPATH "/usr/local/etc/mac_code.list"


union lc{
	unsigned int l;
	unsigned char c[4];
};


class ipaddr{
	private:
		char _c_str[32];
		
	public:
		bit32 _addr;		
		
		ipaddr(){ clear(); }
		ipaddr(const char* str){
			int n = inet_pton(AF_INET, str, &this->_addr);
			if(n == 0){
				fprintf(stderr, "ipaddr(const char*): string error \n");
				exit(-1);
			}else if(n == -1){
				perror("ipaddr(const char*)");
				exit(-1);
			}
		}
		ipaddr(const ipaddr &i){
			this->_addr = i._addr;
		}
		char* c_str(){
			union lc lc;
			lc.l = (unsigned int)this->_addr;
			snprintf(_c_str, sizeof(_c_str)-1,"%u.%u.%u.%u",
							lc.c[0],lc.c[1],lc.c[2],lc.c[3]);
			return _c_str;
		}
		void clear(){
			this->_addr = 0;	
		}
		void setOctet(int n, int num){
			union lc lc;
			if(n>=4){
				fprintf(stderr, "ipaddr::setOctet(): index is not support\n");
				exit(-1);
			}else{
				lc.c[n] = num;
				this->_addr = lc.l;
			}
		}
		bit8 getOctet(int n){
			union lc lc;
			lc.l = this->_addr;
			if(n>=4){
				fprintf(stderr, "ipaddr::operator[]: index is not support\n");
				exit(-1);
			}else{
				return lc.c[n];		
			}
		}
		bool setipbydev(const char* ifname){
			int sockd;
			struct ifreq ifr;
			struct sockaddr_in *sa;
			char* ipstr;

			if ((sockd=socket(AF_INET, SOCK_DGRAM, 0)) < 0){
				perror("ipaddr::setipbydev::socket()");
				clear();
				return false;
			}
			ifr.ifr_addr.sa_family = AF_INET;
			strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
			if(ioctl(sockd, SIOCGIFADDR, &ifr) < 0){
				perror("ipaddr::setipbydev::ioctl()");
				close(sockd);
				clear();
				return false;
			}
			close(sockd);
			sa = (struct sockaddr_in*)&ifr.ifr_addr;
			this->_addr = sa->sin_addr.s_addr;
			return true;
		}
		bool setmaskbydev(const char* ifname){
			int sockd;
			struct ifreq ifr;
			struct sockaddr_in *sa;
			char* maskstr;

			if((sockd=socket(AF_INET, SOCK_DGRAM, 0)) < 0){
				perror("ipaddr::setmacbydev::socket()");
				clear();
				return false;
			}
			ifr.ifr_addr.sa_family = AF_INET;
			strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
			if(ioctl(sockd, SIOCGIFNETMASK, &ifr) < 0){
				perror("ipaddr::setmacbydev::ioctl()");
				close(sockd);
				clear();
				return false;
			}
			close(sockd);
			sa = (struct sockaddr_in*)&ifr.ifr_addr;
			this->_addr = sa->sin_addr.s_addr;
			return true;
		}
		ipaddr& operator=(ipaddr i){
			this->_addr = i._addr;
			return *this;
		}
		ipaddr& operator=(const char* str){
			int n = inet_pton(AF_INET, str, &_addr);
			if(n == 0){
				fprintf(stderr, "ipaddr(const char*): string error \n");
				exit(-1);
			}else if(n == -1){
				perror("ipaddr(const char*)");
				exit(-1);
			}

			return *this;
		}
		ipaddr& operator=(int num){	// FUGUAI ARUKAMO
			this->_addr = num;
			return *this;
		}
		bool operator<(const ipaddr iaddr){
			union lc lc, ilc;
			lc.l = (unsigned int)this->_addr;
			ilc.l = (unsigned int)iaddr._addr;
			
			for(int i=0; i<4; i++){
				if(lc.c[i] == ilc.c[i])	continue;
				else					return lc.c[i] < ilc.c[i];
			}
			return false;
		}
		bool operator>(const ipaddr iaddr){
			union lc lc, ilc;
			lc.l = (unsigned int)this->_addr;
			ilc.l = (unsigned int)iaddr._addr;
			for(int i=0; i<4; i++){
				if(lc.c[i] == ilc.c[i])	continue;
				else					return lc.c[i] > ilc.c[i];
			}
			return false;
		}
		bool operator<=(const ipaddr iaddr){
			union lc lc, ilc;
			lc.l = (unsigned int)this->_addr;
			ilc.l = (unsigned int)iaddr._addr;
			
			for(int i=0; i<4; i++){
				if(lc.c[i] == ilc.c[i])	continue;
				else					return lc.c[i] <= ilc.c[i];
			}
			return false;
		}
		bool operator>=(const ipaddr iaddr){
			union lc lc, ilc;
			lc.l = (unsigned int)this->_addr;
			ilc.l = (unsigned int)iaddr._addr;
			for(int i=0; i<4; i++){
				if(lc.c[i] == ilc.c[i])	continue;
				else					return lc.c[i] >= ilc.c[i];
			}
			return false;
		}
		bool operator==(const ipaddr iaddr){
			return this->_addr == iaddr._addr;
		}
		bool operator!=(const ipaddr iaddr){
			return this->_addr != iaddr._addr;
		}
};




class macaddr{
	private:
		char _c_str[32];
		char _bender[32];
	public:
		bit8 _addr[6];
	
		macaddr(){ clear(); }
		macaddr(const macaddr &m){
			for(int i=0; i<6; i++)
				this->_addr[i] = m._addr[i];
		}
		macaddr(const char* str){
			unsigned int buf[6];
			int n = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x", 
				&buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);
			if(n != 6){
				fprintf(stderr, "macaddr::macaddr(): reading error\n");
				exit(-1);
			}
			for(int i=0; i<6; i++)	this->_addr[i] = (u_char)buf[i];
		}
		void setmacbyarry(const u_char* array){
			for(int i=0; i<6; i++){
				this->_addr[i] = array[i];
			}
		}
		char* c_str(){
			snprintf(_c_str,sizeof(char[18]),"%02x:%02x:%02x:%02x:%02x:%02x",
				this->_addr[0], this->_addr[1], this->_addr[2], 
				this->_addr[3], this->_addr[4], this->_addr[5]);
			return _c_str;
		}
		void clear(){
			memset(this->_addr, 0, sizeof(char[6]));	
		}
		void setOctet(int n, int num){
			if(n>=6){
				fprintf(stderr, "macaddr::setOctet(): index is not support\n");
				exit(-1);
			}else{
				this->_addr[n] = num;
			}
		}
		bit8 getOctet(int n){
			if(n>=6){
				fprintf(stderr, "ipaddr::operator[]: index is not support\n");
				exit(-1);
			}else{
				return this->_addr[n];		
			}
		}
		bool setmacbydev(const char* ifname){
			int sockd;
			struct ifreq ifr;
			u_char addr[6];

			if ((sockd=socket(AF_INET,SOCK_DGRAM,0)) < 0){
				perror("macaddr::setmacbydev::socket()");
				clear();
				return false;
			}
			ifr.ifr_addr.sa_family = AF_INET;
			strncpy(ifr.ifr_name, ifname, IFNAMSIZ-1);
			if(ioctl(sockd, SIOCGIFHWADDR, &ifr) < 0){
				perror("macaddr::setmacbydev::ioctl()");
				close(sockd);
				clear();
				return false;
			}
			close(sockd);
			for(int i=0; i<6; i++)
				this->_addr[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
			return true;		
		}
		char* bender(){
			unsigned int mac[3];
			char  buf[64];
			FILE* fp;
			if((fp=fopen(FILEPATH, "r")) == NULL){
				perror("macaddr::bender()");
				strncpy(_bender, "error", sizeof(_bender)-1);
				return _bender;
			}
			while(fgets(buf, sizeof(buf), fp) != NULL){
				sscanf(buf, "%2x%2x%2x\t%s", &mac[0],&mac[1],&mac[2],buf);
				if(mac[0]==this->_addr[0]&&mac[1]==this->_addr[1]&&mac[2]==this->_addr[2]){
					snprintf(_bender, sizeof(_bender), "%s", buf);
					fclose(fp);
					return _bender;
				}
				memset(mac, 0, sizeof(mac));
				memset(buf, 0, sizeof(buf));
				memset(_bender, 0, sizeof(_bender));
			}
			strncpy(_bender, "not-found", sizeof(_bender)-1);
			fclose(fp);
			return _bender;
		}
		macaddr& operator=(int n){
			for(int i=0; i<6; i++)  this->_addr[i] = n;
			return *this;
		}
		macaddr& operator=(const macaddr m){
			for(int i=0; i<6; i++)	this->_addr[i] = m._addr[i];
			return *this;
		}
		macaddr& operator=(const char* str){
			unsigned int buf[6];
			int n = sscanf(str, "%02x:%02x:%02x:%02x:%02x:%02x", 
				&buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);
			if(n!=6){
				fprintf(stderr, "macaddr::operator=: reading error\n");
				exit(-1);
			}
			for(int i=0; i<6; i++)	this->_addr[i] = (u_char)buf[i];
			return *this;
		}
		bool operator<(const macaddr iaddr){
			for(int i=0; i<6; i++){
				if(this->_addr[i] == iaddr._addr[i])	continue;
				else							return this->_addr[i] < iaddr._addr[i];
			}
			return false;
		}
		bool operator>(const macaddr iaddr){
			for(int i=0; i<6; i++){
				if(this->_addr[i] == iaddr._addr[i])	continue;
				else							return this->_addr[i] > iaddr._addr[i];
			}
			return false;
		}
		bool operator<=(const macaddr iaddr){
			for(int i=0; i<6; i++){
				if(this->_addr[i] == iaddr._addr[i])	continue;
				else							return this->_addr[i] <= iaddr._addr[i];
			}
			return false;
		}
		bool operator>=(const macaddr iaddr){
			for(int i=0; i<6; i++){
				if(this->_addr[i] == iaddr._addr[i])	continue;
				else							return this->_addr[i] >= iaddr._addr[i];
			}
			return false;
		}
		bool operator==(const macaddr iaddr){
			for(int i=0; i<6; i++){
				if(this->_addr[i] != iaddr._addr[i])	return false;
			}
			return true;
		}
		bool operator!=(const macaddr iaddr){
			for(int i=0; i<6; i++){
				if(this->_addr[i] != iaddr._addr[i])	return true;
			}
			return false;
		}
};





#endif /* ADDRESS_H */
