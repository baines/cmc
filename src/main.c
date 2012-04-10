#include "network.h"
#include <SDL/SDL.h>
#include <signal.h>
#include "gfx.h"
#include <math.h>

int run = 1;
float xrot = 0.0f, yrot = 0.0f;
uint32_t delta = 0, time = 0;

void cleanup(int sig){
    run = 0;
}

int main(int argc, char** argv){
    SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_NOPARACHUTE);
    signal(SIGINT, cleanup);
    net_init();
    
    int r = net_connect("localhost", "25565");
    if(r != NET_OK){
        fprintf(stderr, "Couldn't %s host!\n", (r == NET_NOSUCHHOST) ? "resolve" : "connect to");
        exit(1);
    }
    net_send(P_HANDSHAKE, 11, "Test");
    
    gfx_init();
    
    SDL_Event e;
    //int kat = 0;
    
    while(run){
        while(SDL_PollEvent(&e)){
        	switch(e.type){
            	case SDL_QUIT:
               		run = 0;
               	break;
               	case SDL_KEYDOWN:
               		if(e.key.keysym.sym == SDLK_ESCAPE) run = 0;
               		if(e.key.keysym.sym == SDLK_q){
               			SDL_WM_GrabInput(SDL_DISABLE);
               			SDL_ShowCursor(SDL_ENABLE);
               		}
               		if(e.key.keysym.sym == SDLK_e){
               			SDL_WM_GrabInput(SDL_ENABLE);
               			SDL_ShowCursor(SDL_DISABLE);
               		}
               	break;
				case SDL_MOUSEMOTION:
					xrot = fmax(-90.0f, fmin(xrot + (e.motion.yrel / 20.0f), 90.0f));
					yrot += (e.motion.xrel / 20.0f);
					if(yrot < -180.0f) yrot += 360.0f;
					if(yrot > 180.0f) yrot -= 360.0f;
				break;
            }
        }
        net_recv_and_process();
        if(net_get_logged_in()) net_send(P_PLAYER_FLYING, 2, 1);
        gfx_carve_chunks();
        while((delta = (SDL_GetTicks() - time)) < 12){
        	SDL_Delay(1);
        }
        gfx_drawframe(xrot, yrot);
    }
    net_quit();
    SDL_Quit();
    return 0;
}
