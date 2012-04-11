#include "blocks.h"

struct blockinfo blocks[256] = {
	#define B(id, name, tx, ty, flags) \
		[id] = { tx / 16.0f, (tx + 1.0f) / 16.0f, ty / 16.0f, (ty + 1.0f) / 16.0f, flags },
	#include "blocks.txt.h"
	#undef B
};
