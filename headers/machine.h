#include <stdint.h>
#include <SDL.h>
#include "./instruction.h"

#ifndef MACHINE_H
#define MACHINE_H

#define MEM_SIZE 4096
#define STACK_SIZE 16
#define REGISTER_COUNT 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8

struct Machine_s {
  uint8_t SP;
  uint16_t I;
  uint16_t PC;
  uint8_t DELAY_TIMER;
  uint8_t SOUND_TIMER;

  uint8_t REGISTERS[REGISTER_COUNT];
  uint16_t STACK[STACK_SIZE];
  uint8_t MEMORY[MEM_SIZE];

  SDL_Surface* SCREEN;
};

typedef struct Machine_s Machine_t;

void load_file(char* path, Machine_t* machine);

void clear_machine(Machine_t* machine);

void load_font(Machine_t* machine);

void fetch(Machine_t* machine, Instruction_t* instruction);

void sys_init(Machine_t* machine);

#endif