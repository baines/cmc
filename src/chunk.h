#ifndef _CHUNK_H_
#define _CHUNK_H_

struct chunk_mesh;

struct chunk_column {
	int cx, cz;
	uint16_t mask;
	unsigned char const* buff;
	struct chunk_mesh** meshes;
};

void chunk_preload(int cx, int cz);
void chunk_add(int cx, int cz, uint16_t mask, const uint8_t* buff);
uint8_t chunk_getBlockId(struct chunk_column* col, int y, int i);
void chunk_getNeighbors(struct chunk_column* col, int cx, int cz);

#endif
