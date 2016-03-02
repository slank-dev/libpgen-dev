



#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <pgen2/util.h>




namespace pgen {




uint16_t checksum(const void *data, size_t len){
  uint32_t sum = 0; 
  const uint16_t* _data = (const uint16_t*)data;

  for (; len > 1; len -= 2) {
    sum += *_data++;
    if (sum & 0x80000000) 
      sum = (sum & 0xffff) + (sum >> 16);
  }

  if (len == 1) {
    uint16_t i = 0;
    *(uint8_t*) (&i) = *(uint8_t*) _data;
    sum += i;
  }

  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  return ~sum;
}




void hex(const void* buffer, size_t bufferlen) { 

    printf("hexdump len: %lu \n", bufferlen);

    const uint8_t* data = (const uint8_t*)buffer;

    size_t row=0;
    size_t column=0;
    for(row=0; (row+column)<16+bufferlen; row+=16){
        for(column=0; column<=15; column++){
            if(!(row+column < bufferlen)){
                printf("   ");
                if((row+column)%8  == 0 && (row+column)%16 != 0) printf(" ");
            }else{
                if((row+column)%16 == 0) printf("%04x:    ", (unsigned int)(row+column));
                if((row+column)%8  == 0 && (row+column)%16 != 0) printf(" ");
                printf("%02x ", data[row+column]);
            }
        }

        for(column=0; column<=15; column++){
            if(!(row+column < bufferlen)){
                printf(" ");
            }else{
                if((row+column)%16 == 0) 
                    printf("  ");
                if((row+column)%8 == 0 && (row+column)%16!=0) 
                    printf("  ");
                
                if(0x20<=data[row+column] && data[row+column]<=0x7E)
                    printf("%c", data[row+column]);
                else
                    printf(".");
                
                if((row+column+1)%16 == 0)  
                    printf("\n");
            }
        }
    }
    printf("\n");
}


} /* namespace pgen */




