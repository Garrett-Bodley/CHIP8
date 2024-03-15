#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "../headers/machine.h"
#include "../headers/chip8.h"
#include "../headers/decode.h"

#ifdef DEBUG
#include <stdio.h>
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

void chip8(char* path)
{
  static Machine_t machine;
  static Instruction_t instruction;

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