#include "../headers/instruction.h"
#include "../headers/machine.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef APPLE2

#include "../headers/apple2_logic.h"

#endif

void JP(Machine_t* machine, Instruction_t* instruction)
{
  // 1nnn - JP addr
  // Jump to location nnn.

  // The interpreter sets the program counter to nnn.
  #ifdef DEBUG
  puts("JP");
  #endif
  machine->PC = (*instruction)[0] & 0x0F;
  machine->PC <<= 8;
  machine->PC |= (*instruction)[1];
}

void LD_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Set Vx = kk.

  // The interpreter puts the value kk into register Vx.
  (*instruction)[0] &= 0x0F;
  machine->REGISTERS[(*instruction)[0]] = (*instruction)[1];
}

void ADD_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // 7xkk - ADD Vx, byte
  // Set Vx = Vx + kk.

  // Adds the value kk to the value of register Vx, then stores the result in Vx.
  #ifdef DEBUG
  puts("ADD_Vx");
  #endif
  (*instruction)[0] &= 0x0F;
  machine->REGISTERS[(*instruction[0])] += (*instruction)[1];
}

void LD_I(Machine_t* machine, Instruction_t* instruction)
{
  // Annn - LD I, addr
  // Set I = nnn.

  // The value of register I is set to nnn.
  #ifdef DEBUG
  puts("LD_I");
  #endif
  (*instruction)[0] &= 0x0F;
  machine->I = (*instruction)[0];
  machine->I <<= 8;
  machine->I |= (*instruction)[1];
}

void CLS(Machine_t* machine)
{
  // 00E0 - CLS
  // Clear the display.
  #ifdef DEBUG
  puts("CLS");
  #endif
  #ifdef SDL
  memset(machine->SCREEN->pixels, 0x00, SCREEN_REGISTER_COUNT);
  #elif defined(APPLE2)

  char *one = (char *)0x400;
  char *two = (char *)0x480;
  char *three = (char *)0x500;
  char *four = (char *)0x580;
  char *five = (char *)0x600;
  char *six = (char *)0x680;
  char *seven = (char *)0x700;
  char *eight = (char *)0x780;

  memset(one, 0x00, 120);
  memset(two, 0x00, 120);
  memset(three, 0x00, 120);
  memset(four, 0x00, 120);
  memset(five, 0x00, 120);
  memset(six, 0x00, 120);
  memset(seven, 0x00, 120);
  memset(eight, 0x00, 120);

  set_page_2(true);

  memset(one, 0x00, 120);
  memset(two, 0x00, 120);
  memset(three, 0x00, 120);
  memset(four, 0x00, 120);
  memset(five, 0x00, 120);
  memset(six, 0x00, 120);
  memset(seven, 0x00, 120);
  memset(eight, 0x00, 120);

  #endif
}

void RET(Machine_t* machine)
{
  // 00EE - RET
  // Return from a subroutine.

  // The interpreter sets the program counter to the address at the top of the stack,
  // then subtracts 1 from the stack pointer.
  #ifdef DEBUG
  puts("RET");
  #endif
  machine->PC = machine->STACK[machine->SP - 1];
  machine->SP -= 1;
}

void decode_0x0(Machine_t* machine, Instruction_t* instruction)
{
  #ifdef DEBUG
  puts ("Decode 0x0");
  #endif
  switch((*instruction)[1])
  {
    case 0xE0:
      CLS(machine);
      break;
    case 0xEE:
      RET(machine);
  }
}

void DRW_VX_VY(Machine_t* machine, Instruction_t* instruction)
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


  #ifdef SDL
  uint8_t Vx = (*instruction)[0] & 0x0F;
  uint8_t Vy = ((*instruction)[1] & 0xF0) >> 4;
  uint8_t N = (*instruction)[1] & 0x0F;

  uint8_t x = machine->REGISTERS[Vx] & 63;
  uint8_t y = machine->REGISTERS[Vy] & 31;

  // set VF to 0
  machine->REGISTERS[0xF] = 0;

  #ifdef DEBUG
  printf("DRW_VX_VY\n");
  printf("N: %i\n", N);
  printf("I: %04x\n", machine->I);
  printf("I contents: %02x\n", machine->MEMORY[machine->I]);
  // Reading sprite from memory for logging purposes
  for(int i = 0; i < N; i++)
  {
    printf("  %02x\n", machine->MEMORY[machine->I + i]);
  }
  printf("\n");
  #endif

  int x_mem_offset = x / 8; // 8 pixels per word
  int shift_offset = x & 7; // x % 7
  for(int i = 0; i < N; i++)
  {
    uint8_t y_mem_offset = (y + i) * 64 / 8;
    uint8_t mem_offset = x_mem_offset + y_mem_offset;
    uint8_t sprite_word = machine->MEMORY[machine->I + i];

    #ifdef DEBUG
      printf("Sprite word: %02x\n", sprite_word);
    #endif

    if(shift_offset == 0)
    {
      if(machine->REGISTERS[0xF] == 0)
      {
        int mask_res = sprite_word & ((uint8_t*)machine->SCREEN->pixels)[mem_offset];
        if(mask_res != 0){ machine->REGISTERS[0xF] = 1; }
      }
      ((uint8_t*)machine->SCREEN->pixels)[mem_offset] ^= sprite_word;
    }else{
      uint8_t left_mask = sprite_word >> shift_offset;
      uint8_t right_mask = sprite_word << (8 - shift_offset);

      if(x > 55){ right_mask = 0; }
      if(machine->REGISTERS[0xF] == 0)
      {
        uint8_t left_mask_res = left_mask & (((uint8_t*)machine->SCREEN->pixels)[mem_offset] >> shift_offset);
        uint8_t right_mask_res = right_mask & (((uint8_t*)machine->SCREEN->pixels)[mem_offset + 1] << (8 - shift_offset));
        if((left_mask_res | right_mask_res) != 0){ machine->REGISTERS[0xF] = 1; }
      }
      ((uint8_t*)machine->SCREEN->pixels)[mem_offset] ^= left_mask;
      ((uint8_t*)machine->SCREEN->pixels)[mem_offset + 1] ^= right_mask;
    }
  }
  #elif defined(APPLE2)
    // Dxyn - DRW Vx, Vy, nibble
    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

    // The interpreter reads n bytes from memory, starting at the address stored in I.
    // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
    // Sprites are XORed onto the existing screen. If this causes any pixels to be erased,
    // VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is
    // outside the coordinates of the display, it wraps around to the opposite side of the screen.

    // See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more
    // information on the Chip-8 screen and sprites.


    // High nibble is the low row
    // Low nibble is the high row

    // Apple //e double low res is 80 x 48
    // CHIP8 specifies 64 x 32
    //
    // Padding the CHIP 8 screen
    //  8 units on x axis (8 left, 8 right)
    //  8 units on the y axis (8 top, 8 bottom)

    // Compute Apple x,y coordinates based of CHIP8 x, y (I think just add 8?)
    // Determine whether it's the high nibble or low nibble of Apple //e screen
    // Determine if it's Page 1 or Page 2 of screen memory
    // Check for collisions
    // XOR sprite onto screen
    uint8_t i, j, sprite_word, sprite_mask, bit_to_write, val_to_write, x_mem_offset;
    uint16_t y_mem_base;
    uint8_t * address_to_write;
    bool screen_nibble_high;
    bool page2_flag;

    uint8_t Vx = (*instruction)[0] & 0x0F;
    uint8_t Vy = ((*instruction)[1] & 0xF0) >> 4;
    uint8_t N = (*instruction)[1] & 0x0F;
    uint8_t I = machine->I;

    uint8_t x = machine->REGISTERS[Vx] & 63;
    uint8_t y = machine->REGISTERS[Vy] & 31;


    // set VF to 0
    machine->REGISTERS[0xF] = 0;

    // Pad for //e screen
    x += 8;
    y += 8;

    if ((y & 1) == 0)
    {
      screen_nibble_high = false;
    }else{
      screen_nibble_high = true;
    }

    if((x & 1) == 0){
      page2_flag = true;
    }else{
      page2_flag = false;
    }

    for(i = 0; i < N; i++)
    {
      // each i is a row
      // Im going to have to change high or low nibble based on the value of i
      sprite_word = machine->MEMORY[machine->I + i];
      sprite_mask = 128;
      // Write each bit in sprite word to //e screen memory
      for(j = 0; j < 8; j++)
      {
        // each j is a column
        // We don't have to do anything if the bit we're looking at is empty
        bit_to_write = sprite_word & sprite_mask;
        if(bit_to_write == 0) {
          sprite_mask >>= 1;
          page2_flag = !page2_flag;
          ++x;
          continue;
        }

        // Set soft switch to write to the correct memory page
        set_page_2(page2_flag);
        if(screen_nibble_high){
           val_to_write = 0xF0;
        }else{
          val_to_write = 0x0F;
        }

        x_mem_offset = x >> 1;

        if((y >> 1) < 8)
        {
          y_mem_base = FIRST_SCREEN_THIRD;
        }
        else if((y >> 1) < 16)
        {
          y_mem_base = SECOND_SCREEN_THIRD;
        }
        else
        {
          y_mem_base = LAST_SCREEN_THIRD;
        }
        y_mem_base += (((y >> 1) & 7) * SCREEN_ROW_OFFSET);

        address_to_write = (uint8_t*)(y_mem_base + x_mem_offset);

        // Check if VF is set
        if(machine->REGISTERS[0xF] == 0 && (*address_to_write & val_to_write) != 0)
        {
          machine->REGISTERS[0xF] = 1;
        }

        *address_to_write ^= val_to_write;

        // change which page i'm writing to based on the value of J
        sprite_mask >>= 1;
        page2_flag = !page2_flag;
        ++x;
      }
      screen_nibble_high = !screen_nibble_high;
      x -= 8;
      ++y;
    }

  #endif
}

void CALL(Machine_t* machine, Instruction_t* instruction)
{
  // 2nnn - CALL addr
  // Call subroutine at nnn.


  // The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.

  machine->STACK[machine->SP] = machine->PC;
  machine->SP += 1;
  machine->PC = (*instruction)[0] & 0x0F;
  machine->PC <<= 8;
  machine->PC |= (*instruction)[1];
}

void decode(Machine_t* machine, Instruction_t* instruction)
{
  #ifdef DEBUG
    puts("Decoding:");
    printf("PC: %04x\n", machine->PC);
    printf("Instruction: %02x%02x\n", (*instruction)[0], (*instruction)[1]);
  #endif
  uint8_t first_nibble = (*instruction)[0] >> 4;
  switch(first_nibble)
  {
    case 0x0:
      decode_0x0(machine, instruction);
      break;
    case 0x1:
      JP(machine, instruction);
      break;
    case 0x2:
      CALL(machine, instruction);
    case 0x6:
      LD_Vx(machine, instruction);
      break;
    case 0x7:
      ADD_Vx(machine, instruction);
      break;
    case 0xA:
      LD_I(machine, instruction);
      break;
    case 0xD:
      DRW_VX_VY(machine, instruction);
      break;
  }

}