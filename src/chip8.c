#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "../headers/machine.h"
#include "../headers/chip8.h"
#include "../headers/decode.h"
#include "../headers/apple_rom.h"
#include "../headers/apple2_logic.h"

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
  uint16_t i;
  for(i = 0; i < MEM_SIZE; i++)
  {
    if(i % 16 == 0 && i > 0){ printf("\n"); }
    if(i % 16 == 0){ printf("%04x  ", i); }
    printf("%02x " , machine->MEMORY[i]);
    if(i % 16 == 7){ printf(" "); }
  }
  printf("\n");
}
#endif

void log_rom(Machine_t* machine)
{
  uint8_t i;
  // uint8_t* start_position = machine->MEMORY + 0x200;
  for(i = 0; i < rom_size; i++)
  {
    printf("%02x ", machine->MEMORY[0x200 + i]);
  }
}

void log_font(Machine_t* machine)
{
  uint8_t i;
  uint8_t base_addr = 0x50;
  for(i = 0; i < 80; i++)
  {
    printf("%02x\n", machine->MEMORY[base_addr + i]);
  }
}

void chip8(char* path)
{
  volatile uint8_t dummy_val;
  uint8_t i = 0;

  sys_init();
  load_file(path);

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
    fetch();
    #ifdef DEBUG
      printf("Instruction: %02x%02x\n", instruction[0], instruction[1]);
    #endif
    decode();
    quit = sdl_update(&interface);
  }
  #elif defined(APPLE2)
    set_double_low_res();
    CLS();

    while(true)
    {
      if(machine.SOUND_TIMER > 0) dummy_val = *SPEAKER_TOGGLE;
      if(i % 8 == 0){
        if(machine.SOUND_TIMER > 0) --machine.SOUND_TIMER;
        if(machine.DELAY_TIMER > 0) --machine.DELAY_TIMER;
        i = 0;
      }
      fetch();
      decode();
      ++i;
    }
  #endif
}