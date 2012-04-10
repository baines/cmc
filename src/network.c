#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <SDL/SDL.h>
#include "network.h"
#include "net_handle.h"

static void (* const packet_handler[256])(char* buff) = {
	[P_KEEPALIVE]       = net_handle_keepalive,
	[P_LOGIN]           = net_handle_login,
	[P_HANDSHAKE]       = net_handle_handshake,
    [P_CHAT_MSG]        = net_handle_chat,
    [P_TIME_CHANGE]     = net_handle_time_change,
    [P_EQUIP_CHANGE]    = net_handle_equip_change,
    [P_SPAWN_POS]       = net_handle_spawn_pos,
    [P_HEALTH_CHANGE]   = net_handle_health_change,
    [P_RESPAWN]         = net_handle_respawn,
    [P_PLAYER_POS_LOOK] = net_handle_player_pos_look,
    [P_USE_BED]         = net_handle_use_bed,
    [P_ANIMATION]       = net_handle_animation,
    [P_PLAYER_SPAWN]    = net_handle_player_spawn,
    [P_ITEM_SPAWN]      = net_handle_item_spawn,
    [P_ITEM_COLLECT]    = net_handle_item_collect,
    [P_ENTITY_SPAWN]    = net_handle_entity_spawn,
    [P_MOB_SPAWN]       = net_handle_mob_spawn,
    [P_PAINTING]        = net_handle_painting,
    [P_XP_ORB]          = net_handle_xp_orb,
    [P_ENTITY_VELOCITY] = net_handle_entity_velocity,
    [P_ENTITY_DESTROY]  = net_handle_entity_destroy,
    [P_ENTITY]          = net_handle_entity,
    [P_ENTITY_MOVE_REL] = net_handle_entity_move_rel,
    [P_ENTITY_LOOK]     = net_handle_entity_look,
    [P_ENTITY_MR_LOOK]  = net_handle_entity_mr_look,
    [P_ENTITY_TELEPORT] = net_handle_entity_teleport,
    [P_ENTITY_STATUS]   = net_handle_entity_status,
    [P_ENTITY_HEAD]     = net_handle_entity_head,
    [P_ENTITY_ATTACH]   = net_handle_entity_attach,
    [P_ENTITY_DATA]     = net_handle_entity_data,
    [P_ENTITY_FX_START] = net_handle_entity_fx_start,
    [P_ENTITY_FX_STOP]  = net_handle_entity_fx_stop,
    [P_EXPERIENCE]      = net_handle_experience,
    [P_CHUNK_PRELOAD]   = net_handle_chunk_preload,
    [P_CHUNK_LOAD]      = net_handle_chunk_load,
    [P_CHUNK_DIFF]      = net_handle_chunk_diff,
    [P_BLOCK_CHANGE]    = net_handle_block_change,
    [P_BLOCK_ACTION]    = net_handle_block_action,
    [P_EXPLOSION]       = net_handle_explosion,
    [P_SOUND_EFFECT]    = net_handle_sound_effect,
    [P_STATE_CHANGE]    = net_handle_state_change,
    [P_LIGHTNING]       = net_handle_lightning,
    [P_WINDOW_OPEN]     = net_handle_window_open,
    [P_WINDOW_CLOSE]    = net_handle_window_close,
    [P_SLOT_CHANGE]     = net_handle_slot_change,
    [P_SLOTS_CHANGE]    = net_handle_slots_change,
    [P_PROGRESS_BAR]    = net_handle_progress_bar,
    [P_TRANSACTION]     = net_handle_transaction,
    [P_CREATIVE_GIVE]   = net_handle_creative_give,
    [P_SIGN_CHANGE]     = net_handle_sign_change,
    [P_MAP_DATA]        = net_handle_map_data,
    [P_TILE_ENTITY]     = net_handle_tile_entity,
    [P_STATISTIC]       = net_handle_statistic,
    [P_PLAYER_LIST]     = net_handle_player_list,
    [P_PLAYER_PRIVS]    = net_handle_player_privs,
    [P_DISCONNECT]      = net_handle_disconnect
};

struct pollfd pfd;
char *inBuff, *inBuffHead, *inBuffTail;
size_t inBuffSize;
int sock;
int logged_in = 0;

void net_init(){
    inBuff = malloc(BUFF_MAX);
    inBuffHead = inBuff;
    inBuffTail = inBuff;
}

int net_connect(const char* host, char* port){
    struct addrinfo* ai;
    int flag = 1;
   	getaddrinfo(host, port, NULL, &ai);
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return NET_CANTCONNECT;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
	connect(sock, ai->ai_addr, ai->ai_addrlen);
	freeaddrinfo(ai);
	pfd.fd = sock;
	pfd.events = POLLIN;
    return NET_OK;
}

int net_send(uint8_t packet_id, size_t size, ...){
    va_list v;
    va_start(v, size);
    char* buff = malloc(size);
    packet_encode(packet_id, buff, &v);
    va_end(v);
    
    send(sock, buff, size, 0);
    
    free(buff);
    
    return NET_OK;
}

int net_recv(){
    if(poll(&pfd, 1, 0) <= 0)
    	return NET_NOTREADY;
    
    int bytes_read = recv(sock, inBuffTail, BUFF_MAX - (inBuffTail-inBuff), 0);
    
    if(bytes_read <= 0)
        return NET_READERR;
    
    inBuffSize += bytes_read;
    inBuffTail += bytes_read;
    
    return NET_OK;
}

int net_poll_packets(uint8_t* packet_id){
    if(inBuffSize <= 0) return NET_NOMOREDATA;
    int size = packet_size(inBuffHead, inBuffSize);
    if(size == 0 || size > inBuffSize) return NET_NOMOREDATA;
    
    *packet_id = inBuffHead[0];
    inBuffSize -= size;
    
    return size;
}

int net_recv_and_process(){
    while(net_recv() == NET_OK){
        uint8_t p_id;
        int p_size;
        
        while((p_size = net_poll_packets(&p_id))){
            if(packet_handler[p_id] != NULL){
                packet_handler[p_id](inBuffHead+1);
                inBuffHead += p_size;
            } else {
                fprintf(stderr, "Unknown packet id: %#x\n", p_id);
            }
        }
        
        memmove(inBuff, inBuffHead, BUFF_MAX - (inBuffHead - inBuff));
        inBuffTail -= (inBuffHead - inBuff);
        inBuffHead = inBuff;
    }
    return NET_OK;
}

void net_set_logged_in(int val){
	logged_in = val;
}

int net_get_logged_in(void){
	return logged_in;
}

void net_quit(){
    if(sock){
        net_send(P_DISCONNECT, 11, "Cya!");
        SDL_Delay(200);
        close(sock);
    }
    if(inBuff) free(inBuff);
}
       
