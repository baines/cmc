#include <stdlib.h>
#include <stdint.h>

struct chunk_column {
	int cx, cz;
	uint16_t mask;
	unsigned char const* buff;
};

struct chunk_column* columns;
int col_count = 0;

void chunk_add_column(struct chunk_column *col){
	columns = realloc(columns, (col_count + 1) * sizeof(*columns));
	columns[col_count] = *col;
//	gfx_register_chunk_column(columns + col_count);
	++col_count;
}
/*

void chunk_getNeighbours(int x, int z, struct chunk_neighbours* out){

}

*/
