#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <GL/freeglut.h>
#include <time.h>
#define VBO_OFF(i) ((char *)NULL + (i))
#include "blocks.h"
#include "chunk.h"
#include "shader.h"
#include "frustum.h"
#include "assert.h"

struct vx {
	uint8_t x, y, z;
	uint8_t tx;
	uint8_t r, g, b;
	uint8_t align;
};

struct chunk_mesh {
	struct vx* verts;
	GLuint vbo;
	size_t count;
	float x, y, z;
	int cx, cy, cz;
	uint8_t sides[4];
};

struct chunk_mesh* chunks = NULL;

struct chunk_column* newchunks[32];
struct shader shdr;
int nci = 0;
int cur = 0;
int numchunks = 0;

struct lut {
	char v[12];
} lut[6];

unsigned char* keys;
float xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;

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
		{ x + val[0], y + val[1], z + val[2], blocks[i].tx1 , l, l, l },
		{ x + val[3], y + val[4], z + val[5], blocks[i].tx1 , l, l, l },
		{ x + val[6], y + val[7], z + val[8], blocks[i].tx1 , l, l, l },
		{ x + val[9], y + val[10], z + val[11], blocks[i].tx1, l, l, l }
	};
	memcpy(chunks[cur].verts + chunks[cur].count, &v, 4 * sizeof(struct vx));
	chunks[cur].count += 4;
}

void addBillBoard(int x, int y, int z, unsigned char i){
	/*struct vx v[16] = {
		{ x, y+1, z, blocks[i].tx1 blocks[i].ty1, 255, 255, 255 },
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
	chunks[cur].count += 16;*/
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
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	SDL_SetVideoMode(852, 480, 32, SDL_OPENGL);
	
	//glMatrixMode(GL_TEXTURE);
	//glLoadIdentity();
	//glScalef(0.0625f, 0.0625f, 1.0f);
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
	glClearColor(0.4, 0.6, 0.95, 1.0);
	
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_VERTEX_PROGRAM_ARB);
	
	glEnableVertexAttribArrayARB(0);
	glEnableVertexAttribArrayARB(3);
	glEnableVertexAttribArrayARB(8);
	
	shader_load(&shdr, "main");
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
		
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);
	
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
	
	//printf("x: %.2f, y: %.2f, z:%.2f\n", -xpos, -ypos, -zpos);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-8.0f, -64.0f, -8.0f);
	glTranslatef(xpos, ypos, zpos);
	//glPushMatrix();
	//glTranslatef(16.0f, 128.0f, 16.0f);
	frustum_update();
	int j;
	for(j = 0; j < numchunks; ++j){
		if(!frustum_test_sphere(chunks[j].x, chunks[j].y, chunks[j].z, 12.5f)) continue;
		glBindBuffer(GL_ARRAY_BUFFER, chunks[j].vbo);
		glVertexAttribPointerARB(0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct vx), VBO_OFF(0));
		glVertexAttribIPointerEXT(8, 1, GL_UNSIGNED_BYTE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, tx)));
		glVertexAttribPointerARB(3, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, r)));
		glPushMatrix();
		glTranslatef(chunks[j].cx, chunks[j].cy, chunks[j].cz);
		glDrawArrays(GL_QUADS, 0, chunks[j].count);
		glPopMatrix();
	}
	//glPopMatrix();
	SDL_GL_SwapBuffers();
	glPopMatrix();
	//printf("x:%.2f, y:%.2f, z:%.2f\n", -xpos, -ypos, -zpos);
}

void gfx_addchunk(struct chunk_column* col){
	assert(nci < 31);
	++nci;
	newchunks[nci] = col;
}

void gfx_carve_chunks(void){
	for(; nci > 0; --nci){
		uint16_t mask = newchunks[nci]->mask;
		int cx = newchunks[nci]->cx;
		int cz = newchunks[nci]->cz;
		unsigned char const* bptr = newchunks[nci]->buff;
		struct chunk_column neighbors[4];
		chunk_getNeighbors(neighbors, cx, cz);
	
		int mc = __builtin_popcount(mask);
		numchunks += mc;
		chunks = realloc(chunks, numchunks * sizeof(*chunks));
		newchunks[nci]->meshes = malloc(16 * sizeof(struct chunk_mesh*));
		memset(newchunks[nci]->meshes, 0, 16 * sizeof(struct chunk_mesh*));
				
		int i, j, x, y, z, lx, ly, lz, yup, ydn, flags;
		
		for(j = 0; j < 16; ++j){
			if((mask & (1 << j)) == 0) continue;
			yup = (j < 15 && (mask & (1 << (j+1))));
			ydn = (j > 0  && (mask & (1 << (j-1))));
			chunks[cur].cx = cx * 16;
			chunks[cur].cy = j * 16;
			chunks[cur].cz = cz * 16;
			chunks[cur].verts = malloc(0x3000 * sizeof(struct vx));
			chunks[cur].count = 0;
			chunks[cur].x = (cx * 16) + 8.0f;
			chunks[cur].z = (cz * 16) + 8.0f;
			chunks[cur].y = (j * 16) + 8.0f;
			
			for(int c = 0; c < 4; ++c){
				if(neighbors[c].buff) chunks[cur].sides[c] = 1;
				else chunks[cur].sides[c] = 0;
			}
			
			newchunks[nci]->meshes[j] = chunks + cur;
			
			for(i = 0; i < 4096; ++i){
				lx = (i & 0x0F);
				lz = (i & 0xF0) >> 4;
				ly = (i >> 8);
				x = lx;
				z = lz;
				y = ly;
			
				if(!((flags = blocks[bptr[i]].flags) & BF_SOLID)){
					// billboards
					if(flags & BF_BILLBOARD){
						addBillBoard(x, y, z, bptr[i]);
					}
					// water
					if(bptr[i] == 0 && ly > 0x00 && bptr[i-256] == 0x09){
						addQuad(x, y, z, 5, bptr[i-256], 255);
					} 
					// non-edges
					if(lx < 0x0F && blocks[bptr[i+1]].flags & BF_SOLID){
						addQuad(x, y, z, 0, bptr[i+1], 128);
					} else if(lx > 0x00 && blocks[bptr[i-1]].flags & BF_SOLID){
						addQuad(x, y, z, 3, bptr[i-1], 128);
					}
					
					if(lz < 0x0F && blocks[bptr[i+16]].flags & BF_SOLID){
						addQuad(x, y, z, 1, bptr[i+16], 128);
					} else if(lz > 0x00 && blocks[bptr[i-16]].flags & BF_SOLID){
						addQuad(x, y, z, 4, bptr[i-16], 128);
					}
					
					if((yup || ly < 0x0F) && blocks[bptr[i+256]].flags & BF_SOLID){
						addQuad(x, y, z, 2, bptr[i+256], 64);
					} else if((ydn || ly > 0x00) && blocks[bptr[i-256]].flags & BF_SOLID){
						addQuad(x, y, z, 5, bptr[i-256], 255);
					}
					/*
					//edges
					uint8_t o = 0;
					if(lx == 0x0F && blocks[(o = chunk_getBlockId(neighbors + 1, j, i - 15))].flags & BF_SOLID){
						addQuad2(x, y, z, 0, o, 128);
					} else if(lx == 0x00 && blocks[(o = chunk_getBlockId(neighbors + 0, j, i + 15))].flags & BF_SOLID){
						addQuad2(x, y, z, 3, o, 128);
					}
					
					if(lz == 0x0F && blocks[(o = chunk_getBlockId(neighbors + 3, j, i - 240))].flags & BF_SOLID){
						addQuad2(x, y, z, 1, o, 128);
					} else if(lz == 0x00 && blocks[(o = chunk_getBlockId(neighbors + 2, j, i + 240))].flags & BF_SOLID){
						addQuad2(x, y, z, 4, o, 128);
					}*/
					
				} else if(!yup && (ly == 0x0F)){
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
		//free((void*)newchunks[nci].buff);
		printf("%d chunks generated.\n", mc);
	}
}

