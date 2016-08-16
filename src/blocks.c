#include "blocks.h"
#include "gfx.h"

struct blockinfo blocks[256] = {
	#define B(id, name, tx_top, tx_bot, tx_sid, flags) \
		[id] = { tx_top, tx_bot, tx_sid, flags },
	#include "blocks.txt.h"
	#undef B
};

void block_get_colors(uint8_t id, uint8_t side, float* r, float* g, float* b){
	switch(id){
		case B_GRASS: {
			if(side == FACE_UP){
				*r = 0.3f;
				*g = 0.8f;
				*b = 0.3f;
			} else {
				*r = *g = *b = 1.0f;
			}
		} break;

		case B_LEAVES: {
			*r = 0.1f;
			*g = 1.0f;
			*b = 0.1f;
		} break;

		case B_SHRUB: {
			*r = 0.5f;
			*g = 0.9f;
			*b = 0.5f;
		} break;

		default: {
			*r = *g = *b = 1.0f;
		} break;
	}
}


#if 0
unsigned char block_texture_simple(unsigned char block, int side, int meta){
	return blocks[block].tex[side];
}
#endif

/*
unsigned char block_texture_
*/
