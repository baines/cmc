#ifndef _NET_HANDLE_H_
#define _NET_HANDLE_H_

void net_handle_keepalive(char* buffer);
void net_handle_login(char* buffer);
void net_handle_handshake(char* buffer);
void net_handle_chat(char* buffer);
void net_handle_time_change(char* buffer);
void net_handle_equip_change(char* buffer);
void net_handle_spawn_pos(char* buffer);
void net_handle_health_change(char* buffer);
void net_handle_respawn(char* buffer);
void net_handle_player_pos_look(char* buffer);
void net_handle_use_bed(char* buffer);
void net_handle_animation(char* buffer);
void net_handle_player_spawn(char* buffer);
void net_handle_item_spawn(char* buffer);
void net_handle_item_collect(char* buffer);
void net_handle_entity_spawn(char* buffer);
void net_handle_mob_spawn(char* buffer);
void net_handle_painting(char* buffer);
void net_handle_xp_orb(char* buffer);
void net_handle_entity_velocity(char* buffer);
void net_handle_entity_destroy(char* buffer);
void net_handle_entity(char* buffer);
void net_handle_entity_move_rel(char* buffer);
void net_handle_entity_look(char* buffer);
void net_handle_entity_mr_look(char* buffer);
void net_handle_entity_teleport(char* buffer);
void net_handle_entity_head(char* buffer);
void net_handle_entity_status(char* buffer);
void net_handle_entity_attach(char* buffer);
void net_handle_entity_data(char* buffer);
void net_handle_entity_fx_start(char* buffer);
void net_handle_entity_fx_stop(char* buffer);
void net_handle_experience(char* buffer);
void net_handle_chunk_preload(char* buffer);
void net_handle_chunk_load(char* buffer);
void net_handle_chunk_diff(char* buffer);
void net_handle_block_change(char* buffer);
void net_handle_block_action(char* buffer);
void net_handle_explosion(char* buffer);
void net_handle_sound_effect(char* buffer);
void net_handle_state_change(char* buffer);
void net_handle_lightning(char* buffer);
void net_handle_window_open(char* buffer);
void net_handle_window_close(char* buffer);
void net_handle_slot_change(char* buffer);
void net_handle_slots_change(char* buffer);
void net_handle_progress_bar(char* buffer);
void net_handle_transaction(char* buffer);
void net_handle_creative_give(char* buffer);
void net_handle_sign_change(char* buffer);
void net_handle_map_data(char* buffer);
void net_handle_tile_entity(char* buffer);
void net_handle_statistic(char* buffer);
void net_handle_player_list(char* buffer);
void net_handle_player_privs(char* buffer);
void net_handle_disconnect(char* buffer);

#endif
