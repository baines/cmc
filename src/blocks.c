#include "blocks.h"

struct blockinfo blocks[256] = {
	#define B(id, name, tx, ty, flags) \
		[id] = { tx | ((ty << 4) & 0xF0), tx + 1, ty, (ty + 1), flags },
	#include "blocks.txt.h"
	#undef B
};
/*
unsigned char block_texture_simple(unsigned char block, int side, int meta){
	return blocks[block].tex[side];
}

unsigned char block_texture_
*/
