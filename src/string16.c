#include "string16.h"
#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>

inline uint16_t string16_size(char* buffer){
    uint16_t size;
    memcpy(&size, buffer, 2);
    size = be16toh(size);
    
    return size * 2;
}

size_t string16_decode(char** in, char* buffer){
    uint16_t size;
    memcpy(&size, buffer, 2); 
    size = be16toh(size);
    buffer+=2;
    size_t size2 = size * 2;
    size_t s = size2;
    char* conv = malloc(s+1);
    *in = conv;
    
    SDL_iconv_t cd = SDL_iconv_open("UTF-8", "UCS-2BE");
    SDL_iconv(cd, (char const**)&buffer, &size2, &conv, &s);
    SDL_iconv_close(cd);
    
    int end = (size * 2) - s;
    
    (*in)[end] = '\0';
    return (size * 2) + 2;
}

size_t string16_encode(const char* in, char* buffer){
    uint16_t insize = strlen(in);
    uint16_t outsize = insize * 2;
    char* tmp = malloc(outsize);
    char* tmp2 = tmp;
    size_t is = insize;
    size_t os = outsize;
    
    SDL_iconv_t cd = SDL_iconv_open("UCS-2BE", "UTF-8");
    SDL_iconv(cd, &in, &is, &tmp2, &os);
    outsize = outsize - os;
    SDL_iconv_close(cd);
    
    uint16_t size_be = htobe16(insize);
    memcpy(buffer, &size_be, 2); 
    buffer+=2;
    memcpy(buffer, tmp, outsize); 
    buffer+=outsize;
    
    free(tmp);
    
    return outsize + 2;
}

