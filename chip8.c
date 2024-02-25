#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8

uint8_t SP;
uint16_t I;
uint16_t PC;
uint8_t REGISTERS[REGISTER_COUNT];
uint16_t STACK[STACK_SIZE];

uint8_t DELAY_TIMER;
uint8_t SOUND_TIMER;

uint8_t MEMORY[MEM_SIZE];
uint8_t SCREEN[SCREEN_REGISTER_COUNT]; // Bit packed pixels

typedef struct ROM {
  u_int8_t* buffer;
  size_t length;
} ROM;

typedef struct Instruction {
  u_int16_t instruction;
  u_int8_t first;
  u_int8_t first_nibble;
  u_int8_t second;
} Instruction;

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
  rom.length = file_size;

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

  fclose(file);
  return rom;
}

void load_font()
{
  uint8_t font[16][5] = {
    {0xF0, 0x90, 0x90, 0x90, 0xF0}, // 0
    {0x20, 0x60, 0x20, 0x20, 0x70}, // 1
    {0xF0, 0x10, 0xF0, 0x80, 0xF0}, // 2
    {0xF0, 0x10, 0xF0, 0x10, 0xF0}, // 3
    {0x90, 0x90, 0xF0, 0x10, 0x10}, // 4
    {0xF0, 0x80, 0xF0, 0x10, 0xF0}, // 5
    {0xF0, 0x80, 0xF0, 0x90, 0xF0}, // 6
    {0xF0, 0x10, 0x20, 0x40, 0x40}, // 7
    {0xF0, 0x90, 0xF0, 0x90, 0xF0}, // 8
    {0xF0, 0x90, 0xF0, 0x10, 0xF0}, // 9
    {0xF0, 0x90, 0xF0, 0x90, 0x90}, // A
    {0xE0, 0x90, 0xE0, 0x90, 0xE0}, // B
    {0xF0, 0x80, 0x80, 0x80, 0xF0}, // C
    {0xE0, 0x90, 0x90, 0x90, 0xE0}, // D
    {0xF0, 0x80, 0xF0, 0x80, 0xF0}, // E
    {0xF0, 0x80, 0xF0, 0x80, 0x80}  // F
  };

  int char_count = sizeof(font) / sizeof(font[0]);
  int byte_count = sizeof(font[0]) / sizeof(font[0][0]);

  // Via https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#font :
  // "For some reason, it’s become popular to put (the font) at 050–09F"
  int base_address = 0x050;

  for(int i = 0; i < char_count; i++)
  {
    for(int j = 0; j < byte_count; j++)
    {
      MEMORY[base_address + (5 * i) + j] = font[i][j];
    }
  }
}

void sys_init()
{
  load_font();
}

void fetch(Instruction* cur, u_int16_t PC, ROM rom)
{
  uint8_t first = rom.buffer[PC];
  uint8_t second = rom.buffer[PC + 1];

  uint8_t first_nibble_mask = 0xf0;
  uint8_t first_nibble = (first & first_nibble_mask) >> 4;
  uint16_t instruction = 0 | (first << 8) | second;

  cur->first = first;
  cur->second = second;
  cur->first_nibble = first_nibble;
  cur->instruction = instruction;
}

void decode(Instruction cur)
{

}

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  sys_init();

  ROM rom = load_file(argv[1]);
  PC = 0;
  Instruction cur;

  while(PC < rom.length)
  {
    fetch(&cur, PC, rom);
    printf("%04x\n", cur.instruction);
    // decode(cur);
    PC += 2;
  }

  // printf("Rom Length: %zu\n", rom.length);
  // for(int i = 0; i < rom.length; i++){
  //   if(i % 16 == 0){ printf("\n"); }
  //   if(i % 8 == 0){ printf(" "); }
  //   printf("%02x ", rom.buffer[i]);
  // }
  // printf("\n");
  // return 0;

  // for(int i = 0; i < 80; i++)
  // {
  //   if(i % 5 == 0 && i != 0)
  //   {
  //     printf("\n");
  //   }
  //   printf("%x ", MEMORY[0x50 + i]);
  // }
  // printf("\n");
}