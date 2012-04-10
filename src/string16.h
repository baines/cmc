#ifndef _USC2_H_
#define _UCS2_H_
#include "stdint.h"
#include "stddef.h"

uint16_t string16_size(char* buffer);
size_t string16_decode(char** in, char* buffer);
size_t string16_encode(const char* in, char* buffer);

#endif
