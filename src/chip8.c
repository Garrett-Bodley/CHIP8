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
  sys_init(&machine);
  load_file(path, &machine);
  // log_font(&machine);
  // log_rom(&machine);
  // switch_lgd_80(true);
  // CLS(&machine);
  // fill_lgd();

  // apple_sleep(1);
  // switch_text_80(true);
  // apple_sleep(1);
  // char *i = (char *)0x400; // first third of screen memory
  // char *j = (char *)0x428; // second third of screen memory
  // char *k = (char *)0x450; // last third of screen memory

  // // High nibble is the low row
  // // Low nibble is the high row
  // set_double_low_res();
  // memset(i, 0x0f, 935);
  // memset(j, 0x0f, 935);
  // memset(k, 0x0f, 936);
  // set_page_2(true);
  // memset(i, 0x0f, 935);
  // memset(j, 0x0f, 935);
  // memset(k, 0x0f, 936);
  // // CLS(&machine);
  // apple_sleep(1);
  // clear_lgd();
  // uint16_t* iou = 0xC07E;
  // set_double_res(true);

  // printf("IOU Value: %d", (uint8_t)*iou & 128);


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
    fetch(&machine, &instruction);
    #ifdef DEBUG
      printf("Instruction: %02x%02x\n", instruction[0], instruction[1]);
    #endif
    decode(&machine, &instruction);
    quit = sdl_update(&interface);
  }
  #elif defined(APPLE2)
    set_double_low_res();
    CLS(&machine);

    while(true)
    {
      fetch(&machine, &instruction);
      decode(&machine, &instruction);
    }
  #endif
}