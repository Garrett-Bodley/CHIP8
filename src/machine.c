#include <stdint.h>
#include <string.h>
#include "../headers/machine.h"
#ifdef SDL
#include <stdlib.h>
#include <stdio.h>
#elif defined(APPLE2)
#include "../headers/apple_rom.h"
#endif


void clear_machine(Machine_t* machine){
  memset(machine, 0, sizeof(Machine_t));
}

void load_file(char* path, Machine_t* machine)
{
  #ifdef SDL
  FILE *file;
  int file_size;

  file = fopen(path, "rb");

  if(file == NULL)
  {
    perror("Error when opening file");
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  if(file_size > MEM_SIZE - 0x200)
  {
    perror("Provided file is too large");
    exit(2);
  }

  rewind(file);
  uint8_t* start_position = machine->MEMORY + 0x200;

  size_t bytes_read = fread(start_position, sizeof(uint8_t), file_size, file);
  if(bytes_read != file_size)
  {
    perror("Error reading file into virtual machine memory");
    fclose(file);
    exit(3);
  }

  fclose(file);
  #elif defined(APPLE2)
    // rom and rom_size are extern values found in apple_rom.c
    uint8_t* start_position = machine->MEMORY + 0x200;
    memcpy(start_position, &rom, rom_size);
  #endif
}

void load_font(Machine_t* machine)
{
  int i;
  uint8_t FONT[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  uint8_t base_addr = 0x50;
  for(i = 0; i < 80; i++)
  {
    machine->MEMORY[base_addr + i] = FONT[i];
  }
}

void fetch(Machine_t* machine, Instruction_t* instruction)
{
  memcpy(instruction, &machine->MEMORY[machine->PC], 2);
  machine->PC += 2;
}

void sys_init(Machine_t* machine)
{
  clear_machine(machine);
  load_font(machine);
  machine->PC = 0x200;
}