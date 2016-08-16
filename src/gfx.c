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
#include "gfx.h"
#include "blocks.h"
#include "chunk.h"
#include "shader.h"
#include "frustum.h"
#include "assert.h"

#define WIN_W 852
#define WIN_H 480

struct vx {
	uint8_t x, y, z;
	uint8_t tx;
	uint8_t r, g, b;
	uint8_t align;
};

struct chunk_mesh {
	struct vx *verts, *blend_verts;
	GLuint vbo;
	size_t count, blend_count;
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

static GLuint terrain_tex;

void loadTex(void){
	SDL_Surface* surface = IMG_Load("./data/img/terrain.png");
	if (!surface){
		fprintf(stderr, "%s missing or corrupt.\n", "./data/img/terrain.png");
		exit(1);
	}
	glGenTextures(1, &terrain_tex);

	glBindTexture(GL_TEXTURE_2D, terrain_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 4);
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(surface);
}

void addQuad(float x, float y, float z, uint8_t side, uint8_t id, float brightness){
	char* off = lut[side].v;

	float rf, gf, bf;
	block_get_colors(id, side, &rf, &gf, &bf);

	rf *= brightness;
	gf *= brightness;
	bf *= brightness;

	uint8_t r = rf * 255, g = gf * 255, b = bf * 255;

	uint8_t tex = (side == FACE_UP)
		? blocks[id].tex_top
		: side == FACE_DOWN
		? blocks[id].tex_bot
		: blocks[id].tex_side
		;

	struct vx v[4] = {
		{ x + off[0], y + off[1] , z + off[2] , tex, r, g, b },
		{ x + off[3], y + off[4] , z + off[5] , tex, r, g, b },
		{ x + off[6], y + off[7] , z + off[8] , tex, r, g, b },
		{ x + off[9], y + off[10], z + off[11], tex, r, g, b }
	};

	memcpy(chunks[cur].verts + chunks[cur].count, &v, 4 * sizeof(struct vx));
	chunks[cur].count += 4;
}

void addQuadBlend(float x, float y, float z, uint8_t side, uint8_t id, float brightness){
	char* off = lut[side].v;

	float rf, gf, bf;
	block_get_colors(id, side, &rf, &gf, &bf);

	rf *= brightness;
	gf *= brightness;
	bf *= brightness;

	uint8_t r = rf * 255, g = gf * 255, b = bf * 255;

	uint8_t tex = (side == FACE_UP)
		? blocks[id].tex_top
		: side == FACE_DOWN
		? blocks[id].tex_bot
		: blocks[id].tex_side
		;

	struct vx v[4] = {
		{ x + off[0], y + off[1] , z + off[2] , tex, r, g, b },
		{ x + off[3], y + off[4] , z + off[5] , tex, r, g, b },
		{ x + off[6], y + off[7] , z + off[8] , tex, r, g, b },
		{ x + off[9], y + off[10], z + off[11], tex, r, g, b }
	};

	memcpy(chunks[cur].blend_verts + chunks[cur].blend_count, &v, 4 * sizeof(struct vx));
	chunks[cur].blend_count += 4;
}

void addBillBoard(int x, int y, int z, unsigned char id){

	float rf, gf, bf;
	block_get_colors(id, 0, &rf, &gf, &bf);
	uint8_t r = rf * 255, g = gf * 255, b = bf * 255;

	uint8_t tex = blocks[id].tex_side;

	struct vx v[] = {
		{ x  , y+1, z  , tex, r, g, b },
		{ x+1, y+1, z+1, tex, r, g, b },
		{ x+1, y  , z+1, tex, r, g, b },
		{ x  , y  , z  , tex, r, g, b },

		{ x+1, y+1, z+1, tex, r, g, b },
		{ x  , y+1, z  , tex, r, g, b },
		{ x  , y  , z  , tex, r, g, b },
		{ x+1, y  , z+1, tex, r, g, b },

		{ x+1, y+1, z  , tex, r, g, b },
		{ x  , y+1, z+1, tex, r, g, b },
		{ x  , y  , z+1, tex, r, g, b },
		{ x+1, y  , z  , tex, r, g, b },

		{ x  , y+1, z+1, tex, r, g, b },
		{ x+1, y+1, z  , tex, r, g, b },
		{ x+1, y  , z  , tex, r, g, b },
		{ x  , y  , z+1, tex, r, g, b },
	};

	memcpy(chunks[cur].verts + chunks[cur].count, &v, sizeof(v));
	chunks[cur].count += (sizeof(v) / sizeof(*v));
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

	int count = 0;
	glutInit(&count, NULL);

	putenv("SDL_MOUSE_RELATIVE=0");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	SDL_SetVideoMode(WIN_W, WIN_H, 32, SDL_OPENGL);
	SDL_WM_SetCaption("CMC", 0);
	
	//glMatrixMode(GL_TEXTURE);
	//glLoadIdentity();
	//glScalef(0.0625f, 0.0625f, 1.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, WIN_W / (float)WIN_H, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(8.0f, 128.0f, 256.0f, 8.0, 0.0, 8.0, 0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	loadTex();
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glClearColor(0.4, 0.6, 0.95, 1.0);

	glDisable(GL_LIGHTING);

	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_VERTEX_PROGRAM_ARB);
	
	glEnableVertexAttribArrayARB(0);
	glEnableVertexAttribArrayARB(3);
	glEnableVertexAttribArrayARB(8);
	
	shader_load(&shdr, "main");
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GEQUAL, 0.70);
	
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

static int raster_y = 0;

void __attribute__((format (printf, 1, 2))) gfx_debug(const char* fmt, ...){
	va_list v;
	va_start(v, fmt);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), fmt, v);

	glDisable(GL_VERTEX_PROGRAM_ARB);
	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glBindTexture(GL_TEXTURE_2D, 0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WIN_W, WIN_H, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);

	raster_y += 16;
	
	glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
	glRecti(2, raster_y - 12, 8 + 8 * strlen(buffer), raster_y + 4);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glRasterPos2i(6, raster_y);

	for(const char* p = buffer; *p; ++p){
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p);
	}

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);

	va_end(v);
}

struct chunk_mesh* gfx_find_chunk(int x, int y, int z){
	for(int i = 0; i < numchunks; ++i){
		struct chunk_mesh* c = chunks + i;

		int x_lo = c->x - 8, y_lo = c->y - 8, z_lo = c->z - 8;
		int x_hi = c->x + 8, y_hi = c->y + 8, z_hi = c->z + 8;

		if(x >= x_lo && x < x_hi && y >= y_lo && y < y_hi && z >= z_lo && z < z_hi){
			return c;
		}
	}

	return NULL;
}

int chunk_sort_b2f(const void* _a, const void* _b){
	const struct chunk_mesh *a = _a, *b = _b;

	size_t a_dist = abs(a->x + xpos) + abs(a->y + ypos) + abs(a->z + zpos);
	size_t b_dist = abs(b->x + xpos) + abs(b->y + ypos) + abs(b->z + zpos);

	return a_dist - b_dist;
}

void gfx_drawframe(float xrot, float yrot, int delta){
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
	
	raster_y = 0;

	glClearColor(0.4, 0.6, 0.95, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, shdr.v_id);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, shdr.f_id);
	glBindTexture(GL_TEXTURE_2D, terrain_tex);
	
	glPushMatrix();
	glRotatef(xrot, 1.0f, 0.0f, 0.0f);
	glRotatef(yrot, 0.0f, 1.0f, 0.0f);
	//glTranslatef(-8.0f, -64.0f, -8.0f);
	glTranslatef(xpos, ypos, zpos);
	//glPushMatrix();
	//glTranslatef(16.0f, 128.0f, 16.0f);
	frustum_update();

	int num_verts = 0;
	struct chunk_mesh* sorted_chunks[numchunks];

	for(int j = 0; j < numchunks; ++j){
		struct chunk_mesh* c = chunks + j;
		sorted_chunks[j] = c;
		
		if(!frustum_test_sphere(c->x, c->y, c->z, 12.5f)) continue;

		glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
		glVertexAttribPointerARB (0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct vx), VBO_OFF(0));
		glVertexAttribIPointerEXT(8, 1, GL_UNSIGNED_BYTE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, tx)));
		glVertexAttribPointerARB (3, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, r)));

		glPushMatrix();
			glTranslatef(c->cx, c->cy, c->cz);
			glDrawArrays(GL_QUADS, 0, c->count);
		glPopMatrix();
		
		num_verts += c->count;
	}

	qsort(sorted_chunks, numchunks, sizeof(*sorted_chunks), &chunk_sort_b2f);

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);

	for(int j = 0; j < numchunks; ++j){
		struct chunk_mesh* c = sorted_chunks[j];

		if(!frustum_test_sphere(c->x, c->y, c->z, 12.5f)) continue;

		int off = c->count * sizeof(struct vx);

		glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
		glVertexAttribPointerARB (0, 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct vx), VBO_OFF(off));
		glVertexAttribIPointerEXT(8, 1, GL_UNSIGNED_BYTE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, tx)) + off);
		glVertexAttribPointerARB (3, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct vx), VBO_OFF(offsetof(struct vx, r)) + off);

		glPushMatrix();
			glTranslatef(c->cx, c->cy, c->cz);
			glDrawArrays(GL_QUADS, 0, c->blend_count);
		glPopMatrix();
		
		num_verts += c->blend_count;
	}

	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);

	glPopMatrix();

	gfx_debug("Frame: %dms, Verts: %d", delta, num_verts );
	gfx_debug("X: %.2f, Y: %.2f, Z: %.2f", -xpos, -ypos, -zpos);

	struct chunk_mesh* c = gfx_find_chunk(-xpos, -ypos, -zpos);
	if(c){
		gfx_debug("Chunk: %zd, Sides: [%d, %d, %d, %d]", c - chunks, c->sides[0], c->sides[1], c->sides[2], c->sides[3]);

	}

	if(keys[SDLK_f]){
		char* pix = calloc(WIN_W * WIN_H, 4);
		glReadPixels(0, 0, WIN_W, WIN_H, GL_RGBA, GL_UNSIGNED_BYTE, pix);

		FILE* f = fopen("screen.rgba", "wb");
		fwrite(pix, 4, WIN_W * WIN_H, f);
		fflush(f);
		fclose(f);

		free(pix);
	}

	SDL_GL_SwapBuffers();
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
			chunks[cur].blend_verts = malloc(0x3000 * sizeof(struct vx));
			chunks[cur].count = 0;
			chunks[cur].blend_count = 0;
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
						addQuadBlend(x, y, z, FACE_UP, bptr[i-256], 1.0f);
					} 

					// non-edges

					if(lx < 0x0F && blocks[bptr[i+1]].flags & BF_SOLID){
						addQuad(x, y, z, FACE_X_POS, bptr[i+1], 0.5f);
					}
					
					if(lx > 0x00 && blocks[bptr[i-1]].flags & BF_SOLID){
						addQuad(x, y, z, FACE_X_NEG, bptr[i-1], 0.5f);
					}
					
					if(lz < 0x0F && blocks[bptr[i+16]].flags & BF_SOLID){
						addQuad(x, y, z, FACE_Z_POS, bptr[i+16], 0.5f);
					}
					
					if(lz > 0x00 && blocks[bptr[i-16]].flags & BF_SOLID){
						addQuad(x, y, z, FACE_Z_NEG, bptr[i-16], 0.5f);
					}
					
					if((yup || ly < 0x0F) && blocks[bptr[i+256]].flags & BF_SOLID){
						addQuad(x, y, z, FACE_DOWN, bptr[i+256], 0.25f);
					}
					
					if((ydn || ly > 0x00) && blocks[bptr[i-256]].flags & BF_SOLID){
						addQuad(x, y, z, FACE_UP, bptr[i-256], 1.0f);
					}


					// edges

					if(lx == 0 && neighbors[0].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 0, j, i + 15);
						if(blocks[other_id].flags & BF_SOLID){
							addQuad(x, y, z, FACE_X_NEG, other_id, 0.5f);
						}
					}
					
					if(lx == 0xF && neighbors[1].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 1, j, i - 15);
						if(blocks[other_id].flags & BF_SOLID){
							addQuad(x, y, z, FACE_X_POS, other_id, 0.5f);
						}
					}

					if(lz == 0 && neighbors[2].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 2, j, i + 240);
						if(blocks[other_id].flags & BF_SOLID){
							addQuad(x, y, z, FACE_Z_NEG, other_id, 0.5f);
						}
					}

					if(lz == 0xF && neighbors[3].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 3, j, i - 240);
						if(blocks[other_id].flags & BF_SOLID){
							addQuad(x, y, z, FACE_Z_POS, other_id, 0.5f);
						}
					}
					
				} else {
					if(!yup && (ly == 0x0F)){
						addQuad(x, y+1, z, 5, bptr[i], 1.0f);
					}

					if(lx == 0 && neighbors[0].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 0, j, i + 15);
						if(!(blocks[other_id].flags & BF_SOLID)){
							addQuad(x - 1, y, z, FACE_X_POS, bptr[i], 0.5f);
						}
					}
					
					if(lx == 0xF && neighbors[1].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 1, j, i - 15);
						if(!(blocks[other_id].flags & BF_SOLID)){
							addQuad(x + 1, y, z, FACE_X_NEG, bptr[i], 0.5f);
						}
					}

					if(lz == 0 && neighbors[2].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 2, j, i + 240);
						if(!(blocks[other_id].flags & BF_SOLID)){
							addQuad(x, y, z - 1, FACE_Z_POS, bptr[i], 0.5f);
						}
					}

					if(lz == 0xF && neighbors[3].buff){
						uint8_t other_id = chunk_getBlockId(neighbors + 3, j, i - 240);
						if(!(blocks[other_id].flags & BF_SOLID)){
							addQuad(x, y, z + 1, FACE_Z_NEG, bptr[i], 0.5f);
						}
					}
				}
			}
			//chunks[cur].verts = realloc(chunks[cur].verts, chunks[cur].count * sizeof(struct vx));
			
			{
				struct chunk_mesh* c = chunks + cur;
				glGenBuffers(1, &c->vbo);
				glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
				glBufferData(GL_ARRAY_BUFFER, (c->count + c->blend_count) * sizeof(struct vx), NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, c->count * sizeof(struct vx), c->verts);
				glBufferSubData(GL_ARRAY_BUFFER, c->count * sizeof(struct vx), c->blend_count * sizeof(struct vx), c->blend_verts);
				free(c->verts);
				free(c->blend_verts);
			}

			bptr += 4096;
			++cur;
		}
		//free((void*)newchunks[nci].buff);
		printf("%d chunks generated.\n", mc);
	}
}

