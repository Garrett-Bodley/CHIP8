#ifndef MACHINE_H
#define MACHINE_H

#include <stdint.h>
#include "./instruction.h"

#ifdef SDL
#include <SDL.h>
#endif

#define MEM_SIZE 4096
#define STACK_SIZE 16
#define REGISTER_COUNT 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8
#define FONT_BASE 0x50
#define ROM_BASE 0x200

struct Machine_s {
  uint8_t SP;
  uint16_t I;
  uint16_t PC;
  uint8_t DELAY_TIMER;
  uint8_t SOUND_TIMER;

  uint8_t REGISTERS[REGISTER_COUNT];
  uint16_t STACK[STACK_SIZE];
  uint8_t MEMORY[MEM_SIZE];

  #ifdef SDL
  SDL_Surface* SCREEN;
  #elif defined(APPLE2)
  char KEY;
  #endif
};

typedef struct Machine_s Machine_t;

void load_file(char* path);

void clear_machine();

void load_font();

void fetch();

void sys_init();

#endif