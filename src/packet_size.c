#include "packet_size.h"
#include "endian.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "string16.h"
#include "metadata.h"
#include "stdio.h"

size_t packet_size_chunk_load(char* buffer, size_t buffer_size){  
    if(buffer_size < 8) return 0;
    uint32_t tmp;
    memcpy(&tmp, buffer, 4);
    tmp = be32toh(tmp);
    
    if(buffer_size < 8 + tmp) return 0;
    
    return 8 + tmp;
}

size_t packet_size_chunk_diff(char* buffer, size_t buffer_size){
    if(buffer_size < 4) return 0;
    
    uint32_t tmp;
    memcpy(&tmp, buffer, 4);
    tmp = be32toh(tmp);
    
    if(buffer_size < 4 + tmp) return 0;
    
    return 4 + tmp;
}

size_t packet_size_explosion(char* buffer, size_t buffer_size){
    if(buffer_size < 4) return 0;
    
    uint32_t tmp;
    memcpy(&tmp, buffer, 4);
    tmp = be32toh(tmp);
    
    if(buffer_size < 4 + (tmp * 3)) return 0;
    
    return 4 + (tmp * 3);
}

size_t packet_size_slot_change(char* buffer, size_t buffer_size){
	return slot_size(buffer, buffer_size);
}

size_t packet_size_slots_change(char* buffer, size_t buffer_size){
    if(buffer_size < 2) return 0;
    
    int16_t count;
    memcpy(&count, buffer, 2);
    count = be16toh(count);
    
    char* off = buffer + 2;
    
    for(int i = 0; i < count; i++){
		size_t s = slot_size(off, buffer_size - (off - buffer));
		if(s == 0) return 0;
		off += s;
    }
    
    return (off - buffer);
}

size_t packet_size_map_data(char* buffer, size_t buffer_size){
    if(buffer_size < 1) return 0;
    
    uint8_t tmp = (uint8_t)buffer[0];
    if(buffer_size < 1 + tmp) return 0;
    
    return 1 + tmp;
}

size_t packet_size_mod_msg(char* buffer, size_t buffer_size){
	return 0; //FIXME
}

