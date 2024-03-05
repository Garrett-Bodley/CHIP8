#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../headers/machine.h"
#include "../headers/chip8.h"
#include "../headers/load_file.h"

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
  memset(&machine, 0, sizeof(Machine_t));
  load_file(path, &machine);

}