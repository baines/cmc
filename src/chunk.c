#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "chunk.h"
#include "gfx.h"

int minx = INT_MAX, minz = INT_MAX;
int maxx = INT_MIN, maxz = INT_MIN;
int chunk_modx, chunk_addx, chunk_modz, chunk_addz;
struct chunk_column* cstore = NULL;

void chunk_preload(int cx, int cz){
	if(cstore == NULL){
		if(cx < minx) minx = cx;
		else if(cx > maxx) maxx = cx;
	
		if(cz < minz) minz = cz;
		else if(cz > maxz) maxz = cz;
	}
}

uint8_t chunk_getBlockId(struct chunk_column* col, int y, int i){
	if(col == NULL || col->buff == NULL) return 0;
	if(!(col->mask & (1 << y))) return 0;
	
	const uint8_t* ptr = col->buff;
	for(int j = 0; j < y; ++j){
		if(col->mask & (1 << j)) ptr += 4096;
	}
	
	return ptr[i];
}

static void init_cstore(void){
	chunk_modx = 1 + (maxx - minx);
	chunk_addx = 0 - minx;
	
	chunk_modz = 1 + (maxz - minz);
	chunk_addz = 0 - minz;
	
	cstore = malloc(chunk_modx * chunk_modz * sizeof(struct chunk_column));
	memset(cstore, 0, chunk_modx * chunk_modz * sizeof(struct chunk_column));
}

static inline int wrap(int i, int max){
	if(i < 0) return max + i;
	else if(i >= max) return i % max;
	else return i;
}

static inline int xztoindex(int cx, int cz){
	return wrap(cx + chunk_addx, chunk_modx) + (wrap(cz + chunk_addz, chunk_modz) * chunk_modz);
}

static void get_chunk(struct chunk_column* c, int cx, int cz){
	struct chunk_column* p = cstore + xztoindex(cx, cz);
	if(p->buff && p->cx == cx && p->cz == cz)
		memcpy(c, p, sizeof(*p));
}

void chunk_getNeighbors(struct chunk_column* c, int cx, int cz){
	memset(c, 0, sizeof(*c) * 4);
	get_chunk(c + 0, cx -1, cz);
	get_chunk(c + 1, cx +1, cz);
	get_chunk(c + 2, cx, cz -1);
	get_chunk(c + 3, cx, cz +1);
}

void chunk_add(int cx, int cz, uint16_t mask, const uint8_t* buff){
	if(cstore == NULL) init_cstore();
	int i = xztoindex(cx, cz);
	cstore[i].cx = cx;
	cstore[i].cz = cz;
	cstore[i].mask = mask;
	cstore[i].buff = buff;
	
	for(int j = 0; j < (chunk_modx * chunk_modz); ++j){
		printf("%c ", cstore[j].buff ? 'X' : '-');
		if((j % chunk_modz) == (chunk_modz - 1)) printf("\n");
	}
	
	gfx_addchunk(cstore + i);
}
