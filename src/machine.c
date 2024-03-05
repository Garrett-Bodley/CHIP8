#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/machine.h"

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
}