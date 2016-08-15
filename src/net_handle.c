#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <inttypes.h>
#include "net_handle.h"
#include "network.h"
#include "packet.h"
#include "gfx.h"
#include "chunk.h"

void net_handle_keepalive(char* buffer){
    int32_t nonce;
    
    packet_decode(P_KEEPALIVE, buffer, &nonce);
    fprintf(stderr, "KEEPALIVE: %x\n", nonce);
    net_send(P_KEEPALIVE, 5, nonce);
}

void net_handle_login(char* buffer){
    int32_t eid, mode;
    char* empty, *type;
    uint8_t world, difficulty, height, slots;
    
    packet_decode(P_LOGIN, buffer, &eid, &empty, &type, &mode, 
                                      &world, &difficulty, &height, &slots);
    printf("LOGIN: t: %s, gm: %d, w: %d, d: %d, h: %d," 
                "n: %d\n", type, mode, world, difficulty, height, slots);
    net_set_logged_in(1);
    free(empty);
    free(type);
}
void net_handle_handshake(char* buffer){
    char* hash;
    
    packet_decode(P_HANDSHAKE, buffer, &hash);
    fprintf(stderr, "HANDSHAKE: %s\n", hash);
    net_send(P_LOGIN, 20 + 8, 29, "Test", "", 0, 0, 0, 0, 0);
    free(hash);
}
void net_handle_chat(char* buffer){
    unsigned char* msg;
    
    packet_decode(P_CHAT_MSG, buffer, &msg);
    
    const char* ansi = "04261537";
    
    printf("\e[1;37m");
    for(unsigned const char* c = msg; *c; ++c){
        if((*c) != 0xC2 || (*(c+1)) != 0xA7){
            putchar(*c);
            continue;
        }
        c+=2;
        switch(*c){
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                printf("\e[0;3%cm", ansi[(*c) - 48]);
                break;
            case '8':
            case '9':
                printf("\e[1;3%cm", ansi[((*c) - 56)]);
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                printf("\e[1;3%cm", ansi[((*c) - 95)]);
                break;
            default:
                break;
        }
    }
    printf("\e[00m\n");
    free(msg);
}
void net_handle_time_change(char* buffer){
    int64_t time;
    
    packet_decode(P_TIME_CHANGE, buffer, &time);
    fprintf(stderr, "TIME CHANGE: %" PRIi64 "\n", time);
}
void net_handle_equip_change(char* buffer){
    fprintf(stderr, "EQUIP\n");
}
void net_handle_spawn_pos(char* buffer){
    int32_t x, y, z;
    
    packet_decode(P_SPAWN_POS, buffer, &x, &y, &z); 
    fprintf(stderr, "SPAWN POS: x: %d, y: %d, z: %d\n", x, y, z);
}
void net_handle_health_change(char* buffer){
    int16_t hp;
    int16_t food;
    float hunger;

    packet_decode(P_HEALTH_CHANGE, buffer, &hp, &food, &hunger);
    fprintf(stderr, "HEALTH: %hd, %d, %f\n", hp, food, hunger);
}
void net_handle_respawn(char* buffer){
    fprintf(stderr, "RESPAWN\n");
}
void net_handle_player_pos_look(char* buffer){
    double x, y, stance, z;
    float yaw, pitch;
    uint8_t on_ground;
    
    packet_decode(P_PLAYER_POS_LOOK, buffer, &x, &stance, &y, &z, &yaw, 
                                                            &pitch, &on_ground);
    fprintf(stderr, "PosLook: x:%.2f, y:%.2f, z:%.2f, g: %d\n", x, y, z, on_ground);
    net_send(P_PLAYER_POS_LOOK, 42, x, y, stance, z, yaw, pitch, 1);
    gfx_setpos(x, y, z);
}
void net_handle_use_bed(char* buffer){
    fprintf(stderr, "USE BED\n");
}
void net_handle_animation(char* buffer){
    fprintf(stderr, "ANIMATION\n");
}
void net_handle_player_spawn(char* buffer){
    fprintf(stderr, "PLAYER_SPAWN\n");
}
void net_handle_item_spawn(char* buffer){
    fprintf(stderr, "ITEM_SPAWN\n");
}
void net_handle_item_collect(char* buffer){
    fprintf(stderr, "ITEM_COLLECT\n");
}
void net_handle_entity_spawn(char* buffer){
    fprintf(stderr, "ENTITY_SPAWN\n");
}
void net_handle_mob_spawn(char* buffer){
    fprintf(stderr, "MOB_SPAWN\n");
}
void net_handle_painting(char* buffer){
    int32_t id, x, y, z, dir;
    char* title;
    
    packet_decode(P_PAINTING, buffer, &id, &title, &x, &y, &z, &dir);
    fprintf(stdout, "Painting: %s at %d,%d,%d\n", title, x, y, z);
    free(title);
}
void net_handle_xp_orb(char* buffer){
    fprintf(stderr, "XP ORB\n");
}
void net_handle_entity_velocity(char* buffer){
    fprintf(stderr, "ENTITY_VEL\n");
}
void net_handle_entity_destroy(char* buffer){
    fprintf(stderr, "ENTITY_DIE\n");
}
void net_handle_entity(char* buffer){
    fprintf(stderr, "ENTITY\n");
}
void net_handle_entity_move_rel(char* buffer){
    fprintf(stderr, "ENTITY_MOVE_REL\n");
}
void net_handle_entity_look(char* buffer){
    fprintf(stderr, "ENTITY_LOOK\n");
}
void net_handle_entity_mr_look(char* buffer){
    fprintf(stderr, "ENTITY_MR_LOOK\n");
}
void net_handle_entity_teleport(char* buffer){
    fprintf(stderr, "ENTITY TELEPORT\n");
}
void net_handle_entity_head(char* buffer){

}
void net_handle_entity_status(char* buffer){
    fprintf(stderr, "ENTITY STATUS\n");
}
void net_handle_entity_attach(char* buffer){
    fprintf(stderr, "ENTITY ATTACH\n");
}
void net_handle_entity_fx_start(char* buffer){
    fprintf(stderr, "ENTITY FX START\n");
}
void net_handle_entity_fx_stop(char* buffer){
    fprintf(stderr, "ENTITY FX STOP\n");
}
void net_handle_experience(char* buffer){
    fprintf(stderr, "EXPERIENCE\n");
}
void net_handle_entity_data(char* buffer){
    fprintf(stderr, "ENTITY DATA\n");
}
void net_handle_chunk_preload(char* buffer){
	uint32_t x, z;
	uint8_t mode;
	packet_decode(P_CHUNK_PRELOAD, buffer, &x, &z, &mode);
	chunk_preload(x, z);
    fprintf(stderr, "Chunk Preload: %c x: %03d, z: %03d\n", mode == 0 ? '-' : '+', x, z);
}
void net_handle_chunk_load(char* buffer){
    int32_t x, z, size;
    uint16_t mask1, mask2;
    int8_t full;
        
    packet_decode(P_CHUNK_LOAD, buffer, &x, &z, &full, &mask1, &mask2);
    memcpy(&size, buffer + 13, 4);
    size = be32toh(size);
    uint32_t csize = __builtin_popcount(mask1);
	csize *= full ? 12544 : 12288;
    fprintf(stdout, "Chunk: %d - %d\n", size, csize);

	unsigned char *out = malloc(csize);
     
    int ret;
    z_stream strm;
	memset(&strm, Z_NULL, sizeof(strm));
    strm.avail_in = size;
    strm.next_in = (unsigned char*)(buffer + 21);
    strm.avail_out = csize;
    strm.next_out = out;
    ret = inflateInit(&strm);
    ret = inflate(&strm, Z_NO_FLUSH);           
    inflateEnd(&strm);
   
	(void)ret;

    if(full){
	    chunk_add(x, z, mask1, out);
	} else free(out);
}
void net_handle_chunk_diff(char* buffer){
    fprintf(stderr, "CHUNK DIFF\n");
}
void net_handle_block_change(char* buffer){
    fprintf(stderr, "BLOCK CHANGE\n");
}
void net_handle_block_action(char* buffer){
	int32_t x, z;
	int16_t y;
	int8_t b1, b2;
	
	packet_decode(P_BLOCK_ACTION, buffer, &x, &y, &z, &b1, &b2);
    fprintf(stderr, "BLOCK ACTION, x:%d, y:%d, z:%d, (%d, %d)\n", x, y, z, b1, b2);
}
void net_handle_explosion(char* buffer){
    fprintf(stderr, "EXPLOSION\n");
}
void net_handle_sound_effect(char* buffer){
    fprintf(stderr, "SOUND EFFECT\n");
}
void net_handle_state_change(char* buffer){
    fprintf(stderr, "STATE CHANGE\n");
}
void net_handle_lightning(char* buffer){
    fprintf(stderr, "LIGHTNING\n");
}
void net_handle_window_open(char* buffer){
    fprintf(stderr, "WINDOW OPEN\n");
}
void net_handle_window_close(char* buffer){
    fprintf(stderr, "WINDOW CLOSE\n");
}
void net_handle_slot_change(char* buffer){
    fprintf(stderr, "SLOT CHANGE\n");
}
void net_handle_slots_change(char* buffer){
    fprintf(stderr, "SLOTS CHANGE\n");
}
void net_handle_progress_bar(char* buffer){
    fprintf(stderr, "PROGRESS BAR\n");
}
void net_handle_transaction(char* buffer){
    fprintf(stderr, "TRANSACTION\n");
}
void net_handle_creative_give(char* buffer){
    fprintf(stderr, "CREATIVE GIVE\n");
}
void net_handle_sign_change(char* buffer){
    char *l1, *l2, *l3, *l4;
    int32_t x, z;
    int16_t y;
    
    packet_decode(P_SIGN_CHANGE, buffer, &x, &y, &z, &l1, &l2, &l3, &l4);
    fprintf(stdout, "Sign:\n\t%s\n\t%s\n\t%s\n\t%s\n", l1, l2, l3, l4);
    free(l1);
    free(l2);
    free(l3);
    free(l4);
}
void net_handle_map_data(char* buffer){
    fprintf(stderr, "MAP DATA\n");
}
void net_handle_tile_entity(char* buffer){
	fprintf(stderr, "TILE ENTITY\n");
}
void net_handle_statistic(char* buffer){
    fprintf(stderr, "STATISTIC\n");
}
void net_handle_player_list(char* buffer){
	int16_t ms;
	int8_t state;
	char* name;
	packet_decode(P_PLAYER_LIST, buffer, &name, &state, &ms);
    fprintf(stderr, "Player List: %c%s with %d ms\n", state ? '+' : '-', name, ms);
    free(name);
}
void net_handle_player_privs(char* buffer){
    fprintf(stderr, "PLAYER PRIVS\n");
}
void net_handle_disconnect(char* buffer){
    char* reason;
    
    packet_decode(P_DISCONNECT, buffer, &reason);
    printf("DISCONNECTED: \e[1;31m%s.\e[00m\n", reason);
    free(reason);
    exit(1);
}
