#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include <stdint.h>

#define BF_SOLID     (1 << 0)
#define BF_BILLBOARD (1 << 1)
#define BF_BLEND     (1 << 2)

enum {
#define B(id, name, ...) \
	B_##name = id,
#include "blocks.txt.h"
#undef B
};

struct blockinfo {
	uint8_t tex_top, tex_bot, tex_side;
	uint16_t flags;
};

struct block_color {
	uint8_t r, g, b;
};

void block_get_colors(uint8_t id, uint8_t side, float* r, float* g, float* b);

extern struct blockinfo blocks[256];

#endif
