#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include "../headers/sdl_logic.h"
#include "../headers/machine.h"
#include "../headers/chip8.h"
#include "../headers/decode.h"

void log_memory(Machine_t* machine)
{
  for(int i = 0; i < MEM_SIZE; i++)
  {
    if(i % 16 == 0 && i > 0){ printf("\n"); }
    if(i % 16 == 0){ printf("%04x  ", i); }
    printf("%02x " , machine->MEMORY[i]);
    if(i % 16 == 7){ printf(" "); }
  }
  printf("\n");
}

void chip8(char* path)
{
  Machine_t machine;
  Instruction_t instruction;
  sys_init(&machine);
  load_file(path, &machine);
  #ifdef DEBUG
  log_memory(&machine);
  #endif

  SDL_Interface_t interface;
  init_sdl(&interface);

  machine.SCREEN = interface.surface;

  bool quit = false;
  while(!quit)
  {
    fetch(&machine, &instruction);
    #ifdef DEBUG
      printf("Instruction: %02x%02x\n", instruction[0], instruction[1]);
    #endif
    decode(&machine, &instruction);
    quit = sdl_update(&interface);
  }
}