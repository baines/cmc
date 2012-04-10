#include "packet.h"
#include "packet_size.h"
#include "metadata.h"
#include "stdio.h"
#include "endian.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "string16.h"

static const struct Packet packets[256] = {
    #define P(num, name, layout) [name] = { layout },
    #include "packets.txt.h"
    #undef P
};

static size_t (*const packet_size_fn[256])(char* buffer, size_t buffer_size) = {
	[P_CHUNK_LOAD]   = packet_size_chunk_load,
	[P_CHUNK_DIFF]   = packet_size_chunk_diff,
	[P_EXPLOSION]    = packet_size_explosion,
	[P_SLOT_CHANGE]  = packet_size_slot_change,
	[P_SLOTS_CHANGE] = packet_size_slots_change,
	[P_MAP_DATA]     = packet_size_map_data,
	[P_MOD_MSG]      = packet_size_mod_msg
};

size_t packet_size(char* buffer, size_t buffer_size){
	char* origbuff = buffer;
	unsigned char packet_id = buffer[0];
	buffer++;
	for(const char* p = packets[packet_id].layout; *p; p++){
	    switch(*p){
	        case 'c':  {
	            buffer++;
	            break; }
	        case 's':  {
				buffer+=2;
	            break; }
	        case 'i':
	        case 'f':  {
				buffer+=4;
	            break; }
	        case 'l':
	        case 'd':  {
				buffer+=8;
	            break; }
	        case 'z':  {
	        	if(buffer - origbuff > buffer_size - 2){
	        		return 0;
	        	} else {
					buffer+=(2 + string16_size(buffer));
				}
	            break; }
	        case 'm': {
	        	size_t r = meta_size(buffer, buffer_size - (buffer - origbuff));
	        	if(r == 0) return 0;
	        	else buffer += r;
				break; }
	        case '!' : {
	        	size_t r = packet_size_fn[packet_id](buffer, buffer_size - (buffer - origbuff));
	        	if(r == 0) return 0;
	        	else buffer += r;
	        	break; }
	        default:
	            break;
	    }
	}
	size_t s = buffer - origbuff;
	return s > buffer_size ? 0 : s;	
}

void packet_encode(uint8_t packet_id, char* buffer, va_list* v){
    *buffer++ = packet_id;
    
    for(const char* p = packets[packet_id].layout; *p; p++){
        switch(*p){
            case 'c':  {
                *buffer++ = va_arg(*v, int);
                break; }
            case 's':  {
                int16_t s = va_arg(*v, int);
                s = htobe16(s);
                memcpy(buffer, &s, 2); buffer+=2;
                break; }
            case 'i':  {
                int32_t i = va_arg(*v, int);
                i = htobe32(i);
                memcpy(buffer, &i, 4); buffer+=4;
                break; }
            case 'l':  {
                int64_t l = va_arg(*v, long);
                l = htobe64(l);
                memcpy(buffer, &l, 8); buffer+=8;
                break; }
            case 'f':  {
                float f = va_arg(*v, double);
                uint32_t_a i = htobe32(*(uint32_t_a*)&f);
                memcpy(buffer, &i, 4); buffer+=4;
                break; }
            case 'd':  {
                double d = va_arg(*v, double);
                uint64_t_a i = htobe64(*(uint64_t_a*)&d);
                memcpy(buffer, &i, 8); buffer+=8;
                break; }
            case 'z':  {
                const char* arg = va_arg(*v, const char*);
                buffer += string16_encode(arg, buffer);
                break; }
            default:
                break;
        }
    }
}
// net_poll_packets -> packet_handler[packet_id](char* buffer) -> packet_decode
void packet_decode(uint8_t packet_id, char* buffer, ...){
    va_list v;
    va_start(v, buffer);
    
    for(const char* p = packets[packet_id].layout; *p; p++){
        switch(*p){
            case 'c':  {
                int8_t* c = va_arg(v, int8_t*);
                *c = *buffer;
                buffer++;
                break; }
            case 's':  {
                int16_t* s = va_arg(v, int16_t*);
                memcpy(s, buffer, 2); buffer+=2;
                *s = be16toh(*s);
                break; }
            case 'i':  {
                int32_t* i = va_arg(v, int32_t*);
                memcpy(i, buffer, 4); buffer+=4;
                *i = be32toh(*i);
                break; }
            case 'l':  {
                int64_t* l = va_arg(v, int64_t*);
                memcpy(l, buffer, 8); buffer+=8;
                *l = be64toh(*l);
                break; }
            case 'f':  {
                float* f = va_arg(v, float*);
                memcpy(f, buffer, 4); buffer+=4;
                uint32_t_a tmp = htobe32(*(uint32_t_a*)f);
                memcpy(f, &tmp, 4);
                break; }
            case 'd':  {
                double* d = va_arg(v, double*);
                memcpy(d, buffer, 8); buffer+=8;
                uint64_t_a tmp = htobe64(*(uint64_t_a*)d);
                memcpy(d, &tmp, 8);
                break; }
            case 'z':  {
                char** c = va_arg(v, char**);
                buffer+= string16_decode(c, buffer);
                break; }
            case '!':
            case 'm':
                break;
                //packet_decode_special(packet_id, buffer);
        }
    }
    va_end(v);
}

