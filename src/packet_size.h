#ifndef _PACKET_SIZE_H_
#define _PACKET_SIZE_H_
#include "stddef.h"

size_t packet_size_chunk_load(char* buffer, size_t buffer_size);
size_t packet_size_chunk_diff(char* buffer, size_t buffer_size);
size_t packet_size_explosion(char* buffer, size_t buffer_size);
size_t packet_size_slots_change(char* buffer, size_t buffer_size);
size_t packet_size_slot_change(char* buffer, size_t buffer_size);
size_t packet_size_map_data(char* buffer, size_t buffer_size);
size_t packet_size_mod_msg(char* buffer, size_t buffer_size);

#endif
