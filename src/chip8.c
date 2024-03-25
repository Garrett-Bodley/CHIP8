#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "../headers/machine.h"
#include "../headers/chip8.h"
#include "../headers/decode.h"

Machine_t machine;
Instruction_t instruction;

#ifdef DEBUG
#endif


#ifdef SDL
#include <SDL.h>
#include "../headers/sdl_logic.h"
#endif

#ifdef DEBUG
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
#endif

void log_font(Machine_t* machine)
{
  uint8_t i;
  uint8_t base_addr = 0x50;
  for(i = 0; i < 80; i++)
  {
    printf("%02x\n", machine->MEMORY[base_addr + i]);
  }
}

#ifdef SDL
void chip8(char* path)
#elif defined(APPLE2)
void chip8()
#endif
{

  sys_init(&machine);

  #ifdef SDL
  load_file(path, &machine);
  #endif

  #ifdef DEBUG
  log_memory(&machine);
  #endif

  #ifdef SDL
  SDL_Interface_t interface;
  init_sdl(&interface);

  machine.SCREEN = interface.surface;
  #endif

  #ifdef SDL
  bool quit = false;
  while(!quit)
  {
  #else

  #endif
    fetch(&machine, &instruction);
    #ifdef DEBUG
      printf("Instruction: %02x%02x\n", instruction[0], instruction[1]);
    #endif
    decode(&machine, &instruction);
  #ifdef SDL
    quit = sdl_update(&interface);
  }
  #endif
}