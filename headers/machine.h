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

struct Machine_s {
  volatile uint8_t SP;
  volatile uint16_t I;
  volatile uint16_t PC;
  volatile uint8_t DELAY_TIMER;
  volatile uint8_t SOUND_TIMER;

  volatile uint8_t REGISTERS[REGISTER_COUNT];
  volatile uint16_t STACK[STACK_SIZE];
  volatile uint8_t MEMORY[MEM_SIZE];

  #ifdef SDL
  SDL_Surface* SCREEN;
  #endif
};

typedef struct Machine_s Machine_t;

void load_file(char* path, Machine_t* machine);

void clear_machine(Machine_t* machine);

void load_font(Machine_t* machine);

void fetch(Machine_t* machine, Instruction_t* instruction);

void sys_init(Machine_t* machine);

#endif