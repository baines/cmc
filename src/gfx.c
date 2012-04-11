#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <time.h>
#define VBO_OFF(i) ((char *)NULL + (i))
#include "blocks.h"

struct chunkinfo {
	int cx, cz;
	uint16_t mask;
	unsigned char const* buff;
};

struct vx {
	int x, y, z;
	float tx, ty;
	unsigned char r, g, b;
};

struct chunk {
	struct vx* verts;
	GLuint vbo;
	size_t count;
	float x, y, z;
};

struct chunk* chunks = NULL;

struct chunkinfo newchunks[32];
int nci = 0;
int cur = 0;
int numchunks = 0;

struct lut {
	char v[12];
} lut[6];

int SphereInFrustum(float x, float y, float z);
void getFrustum();

unsigned char* keys;
float xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;
float frustum[6][4];

void loadTex(void){
	SDL_Surface* surface = IMG_Load("./data/img/terrain.png");
	if (!surface){
		fprintf(stderr, "%s missing or corrupt.\n", "./data/img/terrain.png");
		exit(1);
	}
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	SDL_FreeSurface(surface);
}

void addQuad(float x, float y, float z, unsigned char j, unsigned char i, unsigned char l){
	char* val = lut[j].v;
	struct vx v[4] = {
		{ x + val[0], y + val[1], z + val[2], blocks[i].tx1, blocks[i].ty1, l, l, l },
		{ x + val[3], y + val[4], z + val[5], blocks[i].tx2, blocks[i].ty1, l, l, l },
		{ x + val[6], y + val[7], z + val[8], blocks[i].tx2, blocks[i].ty2, l, l, l },
		{ x + val[9], y + val[10], z + val[11], blocks[i].tx1, blocks[i].ty2, l, l, l }
	};
	memcpy(chunks[cur].verts + chunks[cur].count, &v, 4 * sizeof(struct vx));
	chunks[cur].count += 4;
}

void addBillBoard(int x, int y, int z, unsigned char i){
	struct vx v[16] = {
		{ x, y+1, z, blocks[i].tx1, blocks[i].ty1, 255, 255, 255 },
		{ x+1, y+1, z+1, blocks[i].tx2, blocks[i].ty1, 255, 255, 255 },
		{ x+1, y, z+1, blocks[i].tx2, blocks[i].ty2, 255, 255, 255 },
		{ x, y, z, blocks[i].tx1, blocks[i].ty2, 255, 255, 255 },
		
		{ x, y, z, blocks[i].tx1, blocks[i].ty2, 255, 255, 255 },
		{ x+1, y, z+1, blocks[i].tx2, blocks[i].ty2, 255, 255, 255 },
		{ x+1, y+1, z+1, blocks[i].tx2, blocks[i].ty1, 255, 255, 255 },
		{ x, y+1, z, blocks[i].tx1, blocks[i].ty1, 255, 255, 255 },
		
		{ x+1, y+1, z, blocks[i].tx1, blocks[i].ty1, 255, 255, 255 },
		{ x, y+1, z+1, blocks[i].tx2, blocks[i].ty1, 255, 255, 255 },
		{ x, y, z+1, blocks[i].tx2, blocks[i].ty2, 255, 255, 255 },
		{ x+1, y, z, blocks[i].tx1, blocks[i].ty2, 255, 255, 255 },
		
		{ x+1, y, z, blocks[i].tx1, blocks[i].ty2, 255, 255, 255 },
		{ x, y, z+1, blocks[i].tx2, blocks[i].ty2, 255, 255, 255 },
		{ x, y+1, z+1, blocks[i].tx2, blocks[i].ty1, 255, 255, 255 },
		{ x+1, y+1, z, blocks[i].tx1, blocks[i].ty1, 255, 255, 255 },
	};
	memcpy(chunks[cur].verts + chunks[cur].count, &v, 16 * sizeof(struct vx));
	chunks[cur].count += 16;
}

void genlut(void){
	uint16_t mask[6] = { 0x37B, 0xB37, 0xF93, 0x816, 0x05A, 0x948 };
	for(int j = 0; j < 6; ++j){
		char* val = lut[j].v;
		for(int i = 0; i < 12; ++i){
			val[i] = ((mask[j] >> i) & 1);
		}
	}
}

void gfx_init(void){
	genlut();
	
	putenv("SDL_MOUSE_RELATIVE=0");
	SDL_SetVideoMode(852, 480, 32, SDL_OPENGL);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 852.0f / 480.0f, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(8.0f, 128.0f, 256.0f, 8.0, 0.0, 8.0, 0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	loadTex();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	SDL_ShowCursor(SDL_DISABLE);
	SDL_WM_GrabInput(SDL_ENABLE);
	//SDL_EventState(SDL_KEYUP | SDL_KEYDOWN, SDL_IGNORE);
	keys = SDL_GetKeyState(NULL);
}

void gfx_setpos(float x, float y, float z){
	xpos = -x;
	ypos = -y;
	zpos = -z;
}

void gfx_drawframe(float xrot, float yrot){
	if(SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE){
		SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse(320, 240);
		SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
	}
	char move1 = keys[SDLK_w] ? 1 : keys[SDLK_s] ? -1 : 0;
	char move2 = keys[SDLK_d] ? 1 : keys[SDLK_a] ? -1 : 0;
	char move3 = keys[SDLK_SPACE] ? 1 : keys[SDLK_LSHIFT] ? -1 : 0;
	
	if(move1 != 0){
		xpos -= move1 * 0.3 * sin(yrot / 180.0f * M_PI);
		zpos += move1 * 0.3 * cos(yrot / 180.0f * M_PI);
		ypos += move1 * 0.3 * sin(xrot / 180.0f * M_PI);
	}
	if(move2 != 0){
		xpos -= move2 * 0.3 * cos(yrot / 180.0f * M_PI);
		zpos -= move2 * 0.3 * sin(yrot / 180.0f * M_PI);
	}
	if(move3 != 0){
		ypos -= 0.3 * move3;
	}
	
	glPushMatrix();
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-8.0f, -64.0f, -8.0f);
	glTranslatef(xpos, ypos, zpos);
	//printf("X: %.2f, Y: %.2f, Z: %.2f\n", -xpos, -ypos, -zpos);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glPushMatrix();
	//glTranslatef(16.0f, 128.0f, 16.0f);
	getFrustum();
	int j;
	for(j = 0; j < numchunks; ++j){
		if(!SphereInFrustum(chunks[j].x, chunks[j].y, chunks[j].z)) continue;
		glBindBuffer(GL_ARRAY_BUFFER, chunks[j].vbo);
		glVertexPointer(3, GL_INT, sizeof(struct vx), VBO_OFF(0));
		glTexCoordPointer(2, GL_FLOAT, sizeof(struct vx), VBO_OFF(offsetof(struct vx, tx)));
		glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, r)));
		glDrawArrays(GL_QUADS, 0, chunks[j].count);
	}
	//glPopMatrix();
	SDL_GL_SwapBuffers();
	glPopMatrix();
	//printf("x:%.2f, y:%.2f, z:%.2f\n", -xpos, -ypos, -zpos);
}

void gfx_addchunk(int cx, int cz, uint16_t mask, unsigned char const* buff){
	struct chunkinfo c = { cx, cz, mask, buff };
	++nci;
	newchunks[nci] = c;
}

void gfx_carve_chunks(void){
	for(; nci > 0; --nci){
		uint16_t mask = newchunks[nci].mask;
		int cx = newchunks[nci].cx;
		int cz = newchunks[nci].cz;	
		unsigned char const* bptr = newchunks[nci].buff;
	
		int mc = __builtin_popcount(mask);
		numchunks += mc;
		chunks = realloc(chunks, numchunks * sizeof(*chunks));
				
		int i, j, x, y, z, yup, ydn, flags;
		
		for(j = 0; j < 16; ++j){
			if((mask & (1 << j)) == 0) continue;
			yup = (j < 15 && (mask & (1 << (j+1))));
			ydn = (j > 0  && (mask & (1 << (j-1))));
			chunks[cur].verts = malloc(0x3000 * sizeof(struct vx));
			chunks[cur].count = 0;
			chunks[cur].x = (cx * 16) + 8.0f;
			chunks[cur].z = (cz * 16) + 8.0f;
			chunks[cur].y = (j * 16) + 8.0f;
			for(i = 0; i < 4096; ++i){
				x = (cx * 16) + (i & 0x0F);
				z = (cz * 16) + ((i & 0xF0) >> 4);
				y = (j * 16) + (i >> 8);
			
				if(!((flags = blocks[bptr[i]].flags) & BF_SOLID)){
				
					if(flags & BF_BILLBOARD){
						addBillBoard(x, y, z, bptr[i]);
					}
					
					if(bptr[i] == 0 && (y & 0x0F) > 0x00 && bptr[i-256] == 0x09){
						addQuad(x, y, z, 5, bptr[i-256], 255);
					} 
					
					if((x & 0x0F) < 0x0F && blocks[bptr[i+1]].flags & BF_SOLID){
						addQuad(x, y, z, 0, bptr[i+1], 128);
					}
					if((z & 0x0F) < 0x0F && blocks[bptr[i+16]].flags & BF_SOLID){
						addQuad(x, y, z, 1, bptr[i+16], 128);
					}
					if((yup || (y & 0x0F) < 0x0F) && blocks[bptr[i+256]].flags & BF_SOLID){
						addQuad(x, y, z, 2, bptr[i+256], 64);
					}
					if((x & 0x0F) > 0x00 && blocks[bptr[i-1]].flags & BF_SOLID){
						addQuad(x, y, z, 3, bptr[i-1], 128);
					}
					if((z & 0x0F) > 0x00 && blocks[bptr[i-16]].flags & BF_SOLID){
						addQuad(x, y, z, 4, bptr[i-16], 128);
					}
					if((ydn || (y & 0x0F) > 0x00) && blocks[bptr[i-256]].flags & BF_SOLID){
						addQuad(x, y, z, 5, bptr[i-256], 255);
					}
				} else if(!yup && ((y & 0x0F) == 0x0F)){
					addQuad(x, y+1, z, 5, bptr[i], 255);
				
				}
			}
			//chunks[cur].verts = realloc(chunks[cur].verts, chunks[cur].count * sizeof(struct vx));
			glGenBuffers(1, &chunks[cur].vbo);
			glBindBuffer(GL_ARRAY_BUFFER, chunks[cur].vbo);
			glBufferData(GL_ARRAY_BUFFER, chunks[cur].count * sizeof(struct vx), chunks[cur].verts, GL_STATIC_DRAW);
			free(chunks[cur].verts);
			bptr += 4096;
			++cur;
		}
		free((void*)newchunks[nci].buff);
		printf("%d chunks generated.\n", mc);
	}
}

int SphereInFrustum(float x, float y, float z){
   const float radius = 12.0f;
   int p;
   
   for(p = 0; p < 6; p++)
      if( frustum[p][0] * x + frustum[p][1] * y + frustum[p][2] * z + frustum[p][3] <= -radius )
         return 0;
   return 1;
}

void getFrustum() {
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   frustum[0][0] = clip[ 3] - clip[ 0];
   frustum[0][1] = clip[ 7] - clip[ 4];
   frustum[0][2] = clip[11] - clip[ 8];
   frustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
   frustum[0][0] /= t;
   frustum[0][1] /= t;
   frustum[0][2] /= t;
   frustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   frustum[1][0] = clip[ 3] + clip[ 0];
   frustum[1][1] = clip[ 7] + clip[ 4];
   frustum[1][2] = clip[11] + clip[ 8];
   frustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
   frustum[1][0] /= t;
   frustum[1][1] /= t;
   frustum[1][2] /= t;
   frustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   frustum[2][0] = clip[ 3] + clip[ 1];
   frustum[2][1] = clip[ 7] + clip[ 5];
   frustum[2][2] = clip[11] + clip[ 9];
   frustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
   frustum[2][0] /= t;
   frustum[2][1] /= t;
   frustum[2][2] /= t;
   frustum[2][3] /= t;

   /* Extract the TOP plane */
   frustum[3][0] = clip[ 3] - clip[ 1];
   frustum[3][1] = clip[ 7] - clip[ 5];
   frustum[3][2] = clip[11] - clip[ 9];
   frustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
   frustum[3][0] /= t;
   frustum[3][1] /= t;
   frustum[3][2] /= t;
   frustum[3][3] /= t;

   /* Extract the FAR plane */
   frustum[4][0] = clip[ 3] - clip[ 2];
   frustum[4][1] = clip[ 7] - clip[ 6];
   frustum[4][2] = clip[11] - clip[10];
   frustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
   frustum[4][0] /= t;
   frustum[4][1] /= t;
   frustum[4][2] /= t;
   frustum[4][3] /= t;

   /* Extract the NEAR plane */
   frustum[5][0] = clip[ 3] + clip[ 2];
   frustum[5][1] = clip[ 7] + clip[ 6];
   frustum[5][2] = clip[11] + clip[10];
   frustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
   frustum[5][0] /= t;
   frustum[5][1] /= t;
   frustum[5][2] /= t;
   frustum[5][3] /= t;
}
