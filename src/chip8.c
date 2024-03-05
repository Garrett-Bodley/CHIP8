#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../headers/machine.h"
#include "../headers/chip8.h"

void load_file(char* path, Machine_t* machine)
{
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
  printf("File Size: %i\n", file_size);
  if(file_size > MEM_SIZE - 0x200)
  {
    perror("Provided file is too large");
    exit(2);
  }

  rewind(file);
  uint8_t* start_position = machine->MEMORY + 0x200;

  size_t bytes_read = fread(start_position, sizeof(uint8_t), file_size, file);
  printf("Bytes Read: %zu\n", bytes_read);
  if(bytes_read != file_size)
  {
    perror("Error reading file into virtual machine memory");
    fclose(file);
    exit(3);
  }

  fclose(file);
}

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
  log_memory(&machine);

}