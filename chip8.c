#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8

uint8_t SP;
uint16_t I;
uint8_t REGISTERS[REGISTER_COUNT];
uint16_t STACK[STACK_SIZE];

uint8_t DELAY_TIMER;
uint8_t SOUND_TIMER;

uint8_t MEMORY[MEM_SIZE];
uint8_t SCREEN[SCREEN_REGISTER_COUNT];

typedef struct ROM {
  u_int8_t* buffer;
  size_t length;
} ROM;

ROM load_file(char *path)
{
  FILE *file;
  int file_size;
  size_t result;

  file = fopen(path, "rb");

  if (file == NULL)
  {
    perror("Error when opening file");
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  ROM rom;
  rom.buffer = malloc(file_size);
  if (rom.buffer == NULL)
  {
    perror("Memory allocation error");
    fclose(file);
    exit(2);
  }

  result = fread(rom.buffer, 1, file_size, file);
  if (result != file_size)
  {
    perror("Error when reading file");
    fclose(file);
    free(rom.buffer);
    exit(3);
  }

  rom.length = file_size;
  fclose(file);
  return rom;
}

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  ROM rom = load_file(argv[1]);

  printf("Rom Length: %zu\n", rom.length);
  for(int i = 0; i < rom.length; i++){
    if(i % 16 == 0){ printf("\n"); }
    if(i % 8 == 0){ printf(" "); }
    printf("%02x ", rom.buffer[i]);
  }
  printf("\n");
  return 0;
  
}