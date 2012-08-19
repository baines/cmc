#ifndef _GFX_H_
#define _GFX_H_

#include "chunk.h"

extern uint8_t* keys;

void gfx_init(void);
void gfx_drawframe(float xrot, float yrot);
void gfx_addchunk(struct chunk_column* col);
void gfx_carve_chunks(void);
void gfx_setpos(float x, float y, float z);

#endif
