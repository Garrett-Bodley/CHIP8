#ifndef SDL_LOGIC_H

#include <SDL.h>
#include <stdbool.h>

#define SDL_LOGIC_h
#define WIDTH 64
#define HEIGHT 32

struct SDL_Interface_s {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
};

typedef struct SDL_Interface_s SDL_Interface_t;

void init_sdl(SDL_Interface_t* interface);

bool sdl_update(SDL_Interface_t* interface);

#endif