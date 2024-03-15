#ifdef SDL

#include <stdbool.h>
#include <SDL.h>
#include "../headers/sdl_logic.h"

void init_sdl(SDL_Interface_t* interface)
{
  int scale = 10;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH * scale, HEIGHT * scale, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_RenderSetScale(renderer, scale, scale);
  SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0 ,WIDTH, HEIGHT, 1, SDL_PIXELFORMAT_INDEX1MSB);

  SDL_Color colors[2];
  colors[0] = (SDL_Color){0x28, 0x29, 0x28, 0xff}; // black background
  colors[1] = (SDL_Color){0x33, 0xff, 0x33, 0xff}; // white fill

  SDL_SetPaletteColors(surface->format->palette, colors, 0, 2);
  interface->renderer = renderer;
  interface->window = window;
  interface->surface = surface;
}

bool sdl_update(SDL_Interface_t* interface)
{
  SDL_Event e;
  while(SDL_PollEvent(&e))
  {
    if(e.type == SDL_QUIT){ return true; }
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(interface->renderer, interface->surface);
  SDL_RenderClear(interface->renderer);
  SDL_RenderCopy(interface->renderer, texture, NULL, NULL);
  SDL_RenderPresent(interface->renderer);
  return false;
}
#endif
