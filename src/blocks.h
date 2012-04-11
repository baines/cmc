#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include <stdint.h>

#define BF_SOLID 1
#define BF_BILLBOARD 2

enum {
#define B(id, name, tx, ty, flags) \
	B_##name = id,
#include "blocks.txt.h"
#undef B
};

struct blockinfo {
	float tx1, tx2, ty1, ty2;
	uint16_t flags;
};

extern struct blockinfo blocks[256];

#endif
