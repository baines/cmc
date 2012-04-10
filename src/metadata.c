#include "metadata.h"
#include "string16.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

size_t meta_size(char* buffer, size_t buffer_size){
    uint8_t x = 0;
    size_t offset = 0;
    
    while(offset < buffer_size){
    	if(offset > buffer_size) return 0;
        x = buffer[offset];
        offset++;
        if(x == 0x7f) break;
        
        switch(x >> 5){
            case 0: offset++;  break;
            case 1: offset+=2; break;
            case 2: offset+=4; break;
            case 3: offset+=4; break;
            case 4: 
                offset++;
                offset+=string16_size(buffer + offset);
                offset++;
                break;
            case 5: offset+=5; break;
            case 6: offset+=12; break;
            default: { printf("Metadata parse error!\n"); exit(1); break; }
        }
    }
    
    return offset;
}

inline size_t slot_size(char* buffer, size_t buffer_size){
	if(buffer_size < 2) return 0;
	int16_t tmp;
	memcpy(&tmp, buffer, 2);
	tmp = be16toh(tmp);
	if(tmp == -1){
		return 2;
	} else {
		if(buffer_size < 5) return 0;
		return 5;
	}
}
