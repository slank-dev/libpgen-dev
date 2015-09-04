#include <stdio.h>
#include "pgen.h"
#include "address.h"
#include "packet.h"
#include "mytypes.h"
#include "netutil.h"


static int get_command_len(const void*);


pgen_ardrone::pgen_ardrone(){
	clear();
}




pgen_ardrone::pgen_ardrone(const u_char* packet, int len){
	clear();
	cast(packet, len);
}



void pgen_ardrone::clear_ctrl(){
	this->ARDRONE.ctrl.seq                = 0;
	this->ARDRONE.ctrl.ctrlmode           = 0;
	this->ARDRONE.ctrl.fw_update_filesize = 0;
}


void pgen_ardrone::clear_pcmd(){
	this->ARDRONE.pcmd.seq   = 0;
	this->ARDRONE.pcmd.flag  = 0;
	this->ARDRONE.pcmd.roll  = 0;
	this->ARDRONE.pcmd.pitch = 0;
	this->ARDRONE.pcmd.gaz   = 0;
	this->ARDRONE.pcmd.yaw.x = 0;
	this->ARDRONE.pcmd.yaw.y = 0;
	this->ARDRONE.pcmd.yaw.z = 0;
}


void pgen_ardrone::clear_ref(){
	this->ARDRONE.ref.seq        = 0;
	this->ARDRONE.ref.command    = 0;
}


void pgen_ardrone::clear(){
	pgen_udp::clear();
	
	clear_ctrl();
	clear_pcmd();
	clear_ref();
	memset(ctrl_data, 0, sizeof(ctrl_data));
	memset(pcmd_data, 0, sizeof(pcmd_data));
	memset(ref_data , 0, sizeof(ref_data));
}



int pgen_ardrone::compile_ctrl(){
	
	snprintf(ctrl_data, sizeof(ctrl_data), "AT*CTRL=%ld,%ld,%ld", this->ARDRONE.ctrl.seq, 
			this->ARDRONE.ctrl.ctrlmode, this->ARDRONE.ctrl.fw_update_filesize);
	return strlen(ctrl_data);
}


int pgen_ardrone::compile_pcmd(){
	snprintf(pcmd_data, sizeof(pcmd_data), "AT*PCMD_MAG=%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld", 
			this->ARDRONE.pcmd.seq, this->ARDRONE.pcmd.flag, 
			this->ARDRONE.pcmd.roll, this->ARDRONE.pcmd.pitch, 
			this->ARDRONE.pcmd.gaz, this->ARDRONE.pcmd.yaw.x, 
			this->ARDRONE.pcmd.yaw.y, this->ARDRONE.pcmd.yaw.z);
	return strlen(pcmd_data);
}


int pgen_ardrone::compile_ref(){
	snprintf(ref_data, sizeof(ref_data),"AT*REF=%ld,%ld", 
			this->ARDRONE.ref.seq, this->ARDRONE.ref.command);
	return strlen(ref_data);	
}




void pgen_ardrone::compile(){
	ctrl_data_len = compile_ctrl();
	pcmd_data_len = compile_pcmd();
	ref_data_len  = compile_ref();
	
	char ardrone[256];
	int ardrone_len = 0;
	char spliter = 0x0d;

	memset(ardrone, 0, sizeof(ardrone));
	char* p1 = ardrone;
	for(int i=0; i<this->ARDRONE.cmd_count; i++){
		if(this->ARDRONE.type[i] == 0){
			memcpy(p1, pcmd_data, pcmd_data_len);
			p1 += pcmd_data_len;
		}else if(this->ARDRONE.type[i] == 1){
			memcpy(p1, ref_data, ref_data_len);
			p1 += ref_data_len;
		}else if(this->ARDRONE.type[i] == 8){
			memcpy(p1, ctrl_data, ctrl_data_len);
			p1 += ctrl_data_len;
		}

		memcpy(p1, &spliter, 1);
		p1 += 1;
	}
	ardrone_len = p1 - ardrone;


	this->UDP.len = UDP_HDR_LEN + ardrone_len;
	pgen_udp::compile();


	memset(this->data, 0, PGEN_MAX_PACKET_LEN);
	u_char* p = this->data;
	memcpy(p, &eth, sizeof eth);
	p += sizeof(eth);
	memcpy(p, &ip, sizeof ip);
	p += sizeof(struct MYIP);
	memcpy(p, &udp, sizeof udp);
	p += sizeof(struct MYUDP);
	memcpy(p, ardrone, ardrone_len);
	p += ardrone_len;
	len = p- this->data;
}



int pgen_ardrone::cast_ctrl(const char* buf){
	sscanf(buf, "AT*CTRL=%ld,%ld,%ld", &this->ARDRONE.ctrl.seq, 
			&this->ARDRONE.ctrl.ctrlmode, &this->ARDRONE.ctrl.fw_update_filesize);
	return get_command_len(buf);
}
int pgen_ardrone::cast_pcmd(const char* buf){
	sscanf(buf, "AT*PCMD_MAG=%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld",
			&ARDRONE.pcmd.seq, &ARDRONE.pcmd.flag, &ARDRONE.pcmd.roll,
			&ARDRONE.pcmd.pitch, &ARDRONE.pcmd.gaz, &ARDRONE.pcmd.yaw.x,
			&ARDRONE.pcmd.yaw.y, &ARDRONE.pcmd.yaw.z);
	return get_command_len(buf);
}
int pgen_ardrone::cast_ref(const char* buf){
	sscanf(buf, "AT*REF=%ld,%ld",
			&this->ARDRONE.ref.seq, &this->ARDRONE.ref.command);
	return get_command_len(buf);
}



void pgen_ardrone::cast(const u_char* packet, int len){
	if(!(this->minLen<=len && len<=this->maxLen)){
		fprintf(stderr, "pgen_tcp::cast(): packet len isn`t support (%d)\n", len);
		return;
	}
	pgen_udp::cast(packet, len);
	
	int cmdlen;
	const char* p = (const char*)packet;
	p += ETH_HDR_LEN;
	p += IP_HDR_LEN;
	p += UDP_HDR_LEN;

	for(this->ARDRONE.cmd_count=0; (const u_char*)p-packet < len; this->ARDRONE.cmd_count++){
		if(strncmp(p, "AT*PCMD", 7) == 0){
			this->ARDRONE.type[this->ARDRONE.cmd_count] = 0;
			//printf("pcmd\n");
			cmdlen = cast_pcmd(p);	
			p += cmdlen + 1;
		}else if(strncmp(p, "AT*REF", 6) == 0){
			this->ARDRONE.type[this->ARDRONE.cmd_count] = 1;
			//printf("ref\n");
			cmdlen = cast_ref(p);
			p += cmdlen + 1;
		}else if(strncmp(p, "AT*CTRL", 7) == 0){
			this->ARDRONE.type[this->ARDRONE.cmd_count] = 8;
			//printf("ctrl\n");
			cmdlen = cast_ctrl(p);
			p += cmdlen + 1;
		}else{
			//printf("other\n");
			fprintf(stderr, "pgen_ardrone::cast: command type not found\n");
			return ;
		}
	}
}




void pgen_ardrone::summary(){
	compile();
	printf("AR Drone len=%d PCMD(seq=%ld flag=%ld roll=%ld pitch=%ld gaz=%ld yaw=(%ld,%ld,%ld)) ", 
			ARDRONE.cmd_count,
			ARDRONE.pcmd.seq, ARDRONE.pcmd.flag, ARDRONE.pcmd.roll, 
			ARDRONE.pcmd.pitch, ARDRONE.pcmd.gaz,
			ARDRONE.pcmd.yaw.x, ARDRONE.pcmd.yaw.y,
			ARDRONE.pcmd.yaw.z);
	printf("REF(seq=%ld, cmd=%ld)\n", ARDRONE.ref.seq, ARDRONE.ref.command);
}




void pgen_ardrone::info(){
	compile();
	pgen_udp::info();
	
	printf(" * AR Drone packet\n");
	printf("    - PCMD MAG\n");
	printf("         Sequence Num : %ld \n", ARDRONE.pcmd.seq);
	printf("         Flag         : %ld \n", ARDRONE.pcmd.flag);
	printf("         Roll         : %ld \n", ARDRONE.pcmd.roll);
	printf("         Pitch        : %ld \n", ARDRONE.pcmd.pitch);
	printf("         Gaz          : %ld \n", ARDRONE.pcmd.gaz);
	printf("         Yaw(x,y,z)   : (%ld,%ld,%ld)  \n", 
			ARDRONE.pcmd.yaw.x, ARDRONE.pcmd.yaw.y,
			ARDRONE.pcmd.yaw.z);
	printf("    - REF\n");
	printf("         Sequence Num : %ld \n", ARDRONE.ref.seq);
	printf("         Command      : %ld \n", ARDRONE.ref.command);
	
}





void pgen_ardrone::DSUMMARY(){
	compile();

	printf("%s -> %s ", IP.src.c_str(), IP.dst.c_str());
	printf("(seq=%ld flag=%ld roll=%ld pitch=%ld gaz=%ld yaw=(%ld,%ld,%ld)) ", 
			ARDRONE.pcmd.seq, ARDRONE.pcmd.flag, ARDRONE.pcmd.roll, 
			ARDRONE.pcmd.pitch, ARDRONE.pcmd.gaz,
			ARDRONE.pcmd.yaw.x, ARDRONE.pcmd.yaw.y,
			ARDRONE.pcmd.yaw.z);
	printf("REF(seq=%ld, cmd=%ld)\n", ARDRONE.ref.seq, ARDRONE.ref.command);
}



static int get_command_len(const void* p){
	const char* c = (const char*)p;
	const char spliter = 0x0d;
	int len;
	for(len=0; c[len]!=spliter; len++) continue;
	
	return len;
}
