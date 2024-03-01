#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

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

void log_screen(){
  for(int i = 0; i < SCREEN_HEIGHT; i++){
    int flag = 0;
    for(int j = 0; j < SCREEN_WIDTH / 8; j++){
      int mem_offset = j + (i * 8);
      printf("%02x", SCREEN[mem_offset]);
      if(flag != 0 ){ printf(" "); }
      flag = ~flag;
    }
    printf("\n");
  }
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
  SP = 0;
  I = 0;
  PC = 0;
  DELAY_TIMER = 0;
  SOUND_TIMER = 0;
  load_font();

  for(int i = 0; i < SCREEN_REGISTER_COUNT; i++){
    SCREEN[i] = 0;
  }

}

void fetch(Instruction* cur, ROM rom)
{
  uint8_t first = rom.buffer[PC];
  uint8_t second = rom.buffer[PC + 1];

  PC += 2;

  uint8_t first_nibble_mask = 0xf0;
  uint8_t first_nibble = (first & first_nibble_mask) >> 4;
  uint16_t instruction = 0 | (first << 8) | second;

  cur->first = first;
  cur->second = second;
  cur->first_nibble = first_nibble;
  cur->instruction = instruction;
}

void clear_screen()
{
  for(int i = 0; i < SCREEN_REGISTER_COUNT; i++)
  {
    SCREEN[i] = 0;
  }
}

void ret()
{
  // 00EE - RET
  // Return from a subroutine.

  // The interpreter sets the program counter to the address at the top of the stack,
  // then subtracts 1 from the stack pointer.

  PC = STACK[SP - 1];
  SP--;
}

void invalid_instruction(Instruction* cur)
{
  printf("Invalid instruction provided\n");
  printf("Instruction: %04x\n", cur->instruction);
  printf("PC: %i\n", PC - 1);
  exit(4);
}

void JP(Instruction* cur)
{
  // 1nnn - JP addr
  // Jump to location nnn.

  // The interpreter sets the program counter to nnn.
  PC = cur->instruction & 0x0fff;
}

void jump_sys_addr(Instruction* cur)
{
  // Jump to a machine code routine at nnn.

  // This instruction is only used on the old computers on which Chip-8 was originally implemented.
  // It is ignored by modern interpreters.
}

void decode_0x0(Instruction* cur)
{
  switch(cur->first)
  {
    case 0x00:
      switch(cur->second)
      {
        case 0xe0:
          clear_screen();
          break;
        case 0xee:
          ret();
          break;
        default:
          invalid_instruction(cur);
      }
    default:
      jump_sys_addr(cur);
      break;

  }
}

void LD_Vx(Instruction* cur)
{
  // 6xkk - LD Vx, byte
  // Set Vx = kk.

  // The interpreter puts the value kk into register Vx.
  uint8_t to_set = cur->first & 0xf;
  REGISTERS[to_set] = cur->second;
}

void ADD_Vx(Instruction* cur)
{
  // 7xkk - ADD Vx, byte
  // Set Vx = Vx + kk.

  // Adds the value kk to the value of register Vx, then stores the result in Vx.

  uint8_t to_set = cur->first & 0xf;
  REGISTERS[to_set] += cur->second;
}

void LD_I(Instruction* cur)
{
  // Annn - LD I, addr
  // Set I = nnn.

  // The value of register I is set to nnn.
  I = cur->instruction & 0x0fff;
}

void DRW_Vx_Vy_N(Instruction* cur)
{
  // Dxyn - DRW Vx, Vy, nibble
  // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

  // The interpreter reads n bytes from memory, starting at the address stored in I.
  // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
  // Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
  // VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is
  // outside the coordinates of the display, it wraps around to the opposite side of the screen.

  // See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more
  // information on the Chip-8 screen and sprites.
  uint8_t Vx = cur->first & 0x0f;
  uint8_t Vy = (cur->second & 0xf0) >> 4;
  uint8_t N = cur->second & 0x0f;

  uint8_t x = REGISTERS[Vx] & 63;
  uint8_t y = REGISTERS[Vy] & 31;

  printf("DRAWING\n");
  printf("Instruction: %04x\n", cur->instruction);
  printf("N: %i\n", N);
  printf("I: %04x\n", I);
  printf("I contents: %02x\n", MEMORY[I]);
  for(int i = 0; i < N; i++)
  {
    printf("  %02x\n", MEMORY[I + i]);
  }
  printf("\n");

  log_screen();

  REGISTERS[0xF] = 0;

  // 64 x 32
  int x_mem_offset = x / 8;
  int shift_offset = x & 7;
  for(int i = 0; i < N; i++)
  {
    int y_mem_offset = (y + i) * SCREEN_WIDTH / 8;
    int mem_offset = x_mem_offset + y_mem_offset;
    int sprite_word = MEMORY[I + i];

    if(shift_offset == 0)
    {
      if(REGISTERS[0xF] == 0)
      {
        int mask_res = sprite_word & SCREEN[mem_offset];
        if(mask_res != 0){ REGISTERS[0xF] = 1; }
      }
      SCREEN[mem_offset] ^= sprite_word;
    }else{
      int left_mask = sprite_word >> shift_offset;
      int right_mask = sprite_word << (8 - shift_offset);

      if(x == 63){ right_mask = 0; }
      if(REGISTERS[0xF] == 0)
      {
        // What if I'm on the edge of the screen? This will wrap in a flat array
        int left_mask_res = left_mask & (SCREEN[mem_offset] >> shift_offset);
        int right_mask_res = right_mask & (SCREEN[mem_offset + 1] << (8 - shift_offset));
        if(left_mask_res != 0 || right_mask_res != 0){ REGISTERS[0xF] = 1; }
      }
      SCREEN[mem_offset] ^= left_mask;
      SCREEN[mem_offset + 1] ^= right_mask;
    }
    printf("\n--------------------------------\n");
    printf("VF: %i\n\n", REGISTERS[0xF]);
    log_screen();
    exit(1);

  }

  // TODO:
  // Do math to figure out what SCREEN addresses (potentially multiple) I need to write to.
  // Then do some bitwise stuff to figure out if I should set the VF register
    // First I need to extract the bits of the pixels I'm writing to
  //  Bitwise & with sprite value
    // If > 0, then set VF = 1 (there are bits that match between values)

  // This is complicated by the fact that the bits I need to set may exist in
  //   multiple virtual registers

  // Also I need to interface with SDL?

  // for(u_int8_t nth_byte = 0; nth_byte < N; nth_byte++)
  // {
  //   uint8_t sprite_row = MEMORY[I + nth_byte];

  //   switch(REGISTERS[0xF])
  //   {
  //     case 0:
  //       if(~sprite_row != SCREEN[])
  //       MEMORY[I + nth_byte];
  //       break;
  //     case 1:
  //       break;
  //   }
  // }
}

void decode(Instruction* cur)
{
  switch(cur->first_nibble)
  {
    case 0x0:
      decode_0x0(cur);
      break;
    case 0x1:
      JP(cur);
      break;
    case 0x2:
      break;
    case 0x6:
      LD_Vx(cur);
      break;
    case 0x7:
      ADD_Vx(cur);
      break;
    case 0xa:
      LD_I(cur);
      break;
    case 0xd:
      DRW_Vx_Vy_N(cur);
      break;

  }
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
    fetch(&cur, rom);
    // printf("%04x\n", cur.instruction);
    decode(&cur);
  }

  // for(int i = 0; i < SCREEN_REGISTER_COUNT; i++){
  //   printf("%04x ", SCREEN[i]);
  //   printf("\n");
  // }
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
  printf("Yay!\n");
}