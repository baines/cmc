#ifndef _GFX_H_
#define _GFX_H_

void gfx_init(void);
void gfx_drawframe(float xrot, float yrot);
void gfx_addchunk(int cx, int cz, uint16_t mask, unsigned char* buff);
void gfx_carve_chunks(void);
void gfx_setpos(float x, float y, float z);

#endif
