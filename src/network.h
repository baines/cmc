#ifndef _NETWORK_H_
#define _NETWORK_H_
#include "packet.h"
#define BUFF_MAX 8192

enum {
	NET_NOTREADY = -4,
	NET_NOSUCHHOST = -3,
	NET_CANTCONNECT = -2,
	NET_READERR = -1,
	NET_NOMOREDATA = 0,
	NET_OK = 1
};	

void net_init();
int net_connect(const char* host, char* port);
int net_send(uint8_t packet_id, size_t size, ...);
int net_recv_and_process();
void net_quit();
int net_get_logged_in(void);
void net_set_logged_in(const int val);

#endif
