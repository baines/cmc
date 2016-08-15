#ifndef _GFX_H_
#define _GFX_H_

#include "chunk.h"

extern uint8_t* keys;

void gfx_init(void);
void gfx_drawframe(float xrot, float yrot, int delta);
void gfx_addchunk(struct chunk_column* col);
void gfx_carve_chunks(void);
void gfx_setpos(float x, float y, float z);

enum {
	FACE_X_POS = 0,
	FACE_Z_POS,
	FACE_Y_POS,
	FACE_X_NEG,
	FACE_Z_NEG,
	FACE_Y_NEG,

	FACE_UP   = FACE_Y_NEG,
	FACE_DOWN = FACE_Y_POS,
};

#endif
