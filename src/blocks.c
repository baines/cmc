#include "blocks.h"

struct blockinfo blocks[256] = {
	#define B(id, name, tx, ty, flags) \
		[id] = { tx, tx + 1, ty, ty + 1, flags },
	#include "blocks.txt.h"
	#undef B
};
