#include "network.h"
#include <SDL/SDL.h>
#include <signal.h>
#include "gfx.h"
#include <math.h>
#include <sys/resource.h>

int run = 1;
int mx = 426, my = 240, show_cur = 0;
float xrot = 0.0f, yrot = 0.0f;
uint32_t delta = 0, time = 0;

void cleanup(int sig){
    run = 0;
}

int main(int argc, char** argv){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);
    signal(SIGINT, cleanup);
    net_init();
    
    int r = net_connect("localhost", "25565");
    if(r != NET_OK){
        fprintf(stderr, "Couldn't %s host!\n", (r == NET_NOSUCHHOST) ? "resolve" : "connect to");
        exit(1);
    }
    net_send(P_HANDSHAKE, 11, "Test");
    
    gfx_init();
    
    SDL_WarpMouse(mx, my);
    
    SDL_Event e;
    memset(&e, 0, sizeof(e));
    
    while(run){
        while(SDL_PollEvent(&e)){
        	switch(e.type){
            	case SDL_QUIT:
               		run = 0;
               	break;
               	case SDL_KEYDOWN:
               		if(e.key.keysym.sym == SDLK_ESCAPE) run = 0;
               		if(e.key.keysym.sym == SDLK_m){
               			struct rusage r;
               			getrusage(RUSAGE_SELF, &r);
               			printf("*** MEM USAGE: %ld MB ***\n", r.ru_maxrss / 1024);
               		}
               		if(e.key.keysym.sym == SDLK_q){
               			SDL_WM_GrabInput(SDL_DISABLE);
               			SDL_ShowCursor(SDL_ENABLE);
               			show_cur = 1;
               		}
               		if(e.key.keysym.sym == SDLK_e){
               			SDL_WM_GrabInput(SDL_ENABLE);
               			SDL_ShowCursor(SDL_DISABLE);
               			show_cur = 0;
               		}
               	break;
				case SDL_MOUSEMOTION:
					if(mx == 426 && my == 240 && e.motion.x == 426 && e.motion.y == 240) break;
					xrot = fmax(-90.0f, fmin(xrot + ((e.motion.y - my) / 20.0f), 90.0f));
					yrot += ((e.motion.x - mx) / 20.0f);
					mx = e.motion.x;
					my = e.motion.y;
					if(yrot < -180.0f) yrot += 360.0f;
					if(yrot > 180.0f) yrot -= 360.0f;
				break;
            }
        }
        /*int mx2, my2;
        SDL_GetMouseState(&mx2, &my2);
        mx = mx2 - mx;
        my = my2 - my;
		xrot = fmax(-90.0f, fmin(xrot + (my / 20.0f), 90.0f));
		yrot += (mx / 20.0f);
		if(yrot < -180.0f) yrot += 360.0f;
		if(yrot > 180.0f) yrot -= 360.0f; */
		if(!show_cur){
			mx = 426;
			my = 240;
			SDL_WarpMouse(mx, my);
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
