#ifndef _PACKETS_H_
#define _PACKETS_H_
#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"

enum _Packets {
#define P(num, name, layout) name = num,
#include "packets.txt.h"
#undef P
};

struct Packet {
    const char* layout;
};

typedef uint32_t __attribute__((__may_alias__)) uint32_t_a;
typedef uint64_t __attribute__((__may_alias__)) uint64_t_a;

size_t packet_size(char* buffer, size_t buffer_size);
void packet_encode(uint8_t packet_id, char* buffer, va_list* v);
void packet_decode(uint8_t packet_id, char* buffer, ...);
#endif



