#ifndef _SHADER_H_
#define _SHADER_H_

#include <stdint.h>

struct shader {
	uint32_t v_id, f_id;
};

void shader_load(struct shader* s, const char* basename);

#endif
