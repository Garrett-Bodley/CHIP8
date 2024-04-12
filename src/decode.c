#include "../headers/instruction.h"
#include "../headers/machine.h"
#include <stdint.h>
#include <stdlib.h>
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

void SYS(Machine_t* machine)
{
  // 0nnn - SYS addr
  // Jump to a machine code routine at nnn.

  // This instruction is only used on the old computers on which Chip-8 was originally implemented. It is ignored by modern interpreters.

  // noop
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
      break;
    default:
      SYS(machine);
      break;
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

void LD_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy0 - LD Vx, Vy
  // Set Vx = Vy.

  // Stores the value of register Vy in register Vx.

  machine->REGISTERS[Vx] = machine->REGISTERS[Vy];
}

void OR_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy1 - OR Vx, Vy
  // Set Vx = Vx OR Vy.

  // Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the
  // corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1.
  // Otherwise, it is 0.

  machine->REGISTERS[Vx] |= machine->REGISTERS[Vy];
}

void AND_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy2 - AND Vx, Vy
  // Set Vx = Vx AND Vy.

  // Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the
  // corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1.
  // Otherwise, it is 0.

  machine->REGISTERS[Vx] &= machine->REGISTERS[Vy];
}

void XOR_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy3 - XOR Vx, Vy
  // Set Vx = Vx XOR Vy.

  // Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR compares the
  // corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1.
  // Otherwise, it is 0.

  machine->REGISTERS[Vx] ^= machine->REGISTERS[Vy];
}

void ADD_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy4 - ADD Vx, Vy
  // Set Vx = Vx + Vy, set VF = carry.

  // The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
  // Only the lowest 8 bits of the result are kept, and stored in Vx.

  // Must do some trickery because C integers "roll over"
  uint16_t result = machine->REGISTERS[Vx] + machine->REGISTERS[Vy];
  if(result >> 8 > 0)
  {
    machine->REGISTERS[0xF] = 1;
  }else{
    machine->REGISTERS[0xF] = 0;
  }
  machine->REGISTERS[Vx] = (uint8_t)(result & 0xFF);
}

void SUB_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy5 - SUB Vx, Vy
  // Set Vx = Vx - Vy, set VF = NOT borrow.

  // If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
  if(machine->REGISTERS[Vx] > machine->REGISTERS[Vy])
  {
    machine->REGISTERS[0xF] = 1;
  }else{
    machine->REGISTERS[0xF] = 0;
  }
  machine->REGISTERS[Vx] -= machine->REGISTERS[Vy];
}

void SUBN_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy)
{
  // 8xy7 - SUBN Vx, Vy
  // Set Vx = Vy - Vx, set VF = NOT borrow.

  // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy,
  // and the results stored in Vx.

  if(machine->REGISTERS[Vy] > machine->REGISTERS[Vx])
  {
    machine->REGISTERS[0xF] = 0x01;
  }else{
    machine->REGISTERS[0xF] = 0x00;
  }

  machine->REGISTERS[Vx] = machine->REGISTERS[Vy] - machine->REGISTERS[Vx];
}

void SHR_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy){
  // This honors the original CHIP-8 spec.
  // NOT COMPATIBLE WITH CHIP-48 OR SUPER-CHIP

  // 8xy6 - SHR Vx, Vy
  // Set Vx = Vy SHR 1.

  // First put the value in Vy in Vx.
  // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.

  machine->REGISTERS[Vx]= machine->REGISTERS[Vy];
  if((machine->REGISTERS[Vx] & 1) > 0){
    machine->REGISTERS[0xF] = 1;
  }else{
    machine->REGISTERS[0xF] = 0;
  }
  machine->REGISTERS[Vx] >>= 1;
}

void SHL_Vx_Vy(Machine_t* machine, uint8_t Vx, uint8_t Vy){
  // This honors the original CHIP-8 spec.
  // NOT COMPATIBLE WITH CHIP-48 OR SUPER-CHIP

  // 8xyE - SHL Vx, Vy
  // Set Vx = Vx SHL 1.

  // First put the value in Vy in Vx.
  // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.

  machine->REGISTERS[Vx]= machine->REGISTERS[Vy];
  if((machine->REGISTERS[Vx] & 0x80) > 0){
    machine->REGISTERS[0xF] = 1;
  }else{
    machine->REGISTERS[0xF] = 0;
  }
  machine->REGISTERS[Vx] &= 0x7f; // Clear MSB to prevent overflow/"roll over" behavior
  machine->REGISTERS[Vx] <<= 1;
}

void decode_ALU(Machine_t* machine, Instruction_t* instruction)
{
  uint8_t Vx, Vy, low_nibble;

  low_nibble = (*instruction)[1] & 0xF;
  Vx = (*instruction)[0] & 0xF;
  Vy = ((*instruction)[1] & 0xF0) >> 4;

  switch(low_nibble)
  {
    case 0x0:
      LD_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x1:
      OR_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x2:
      AND_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x3:
      XOR_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x4:
      ADD_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x5:
      SUB_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x6:
      SHR_Vx_Vy(machine, Vx, Vy);
      break;
    case 0x7:
      SUBN_Vx_Vy(machine, Vx, Vy);
      break;
    case 0xe:
      SHL_Vx_Vy(machine, Vx, Vy);
      break;
  }
}

void SE_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // 3xkk - SE Vx, byte
  // Skip next instruction if Vx = kk.

  // The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.

  uint8_t Vx = (*instruction)[0] & 0x0F;
  if(machine->REGISTERS[Vx] == (*instruction)[1])
  {
    machine->PC += 2;
  }
}

void SNE_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // 4xkk - SNE Vx, byte
  // Skip next instruction if Vx != kk.

  // The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.

  uint8_t Vx = (*instruction)[0] & 0x0F;
  if(machine->REGISTERS[Vx] != (*instruction)[1])
  {
    machine->PC += 2;
  }
}

void SE_Vx_Vy(Machine_t* machine, Instruction_t* instruction)
{
  // 5xy0 - SE Vx, Vy
  // Skip next instruction if Vx = Vy.

  // The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.

  uint8_t Vx = (*instruction)[0] & 0x0F;
  uint8_t Vy = (((*instruction)[1]) & 0xF0) >> 4;
  if(machine->REGISTERS[Vx] == machine->REGISTERS[Vy])
  {
    machine->PC += 2;
  }
}

void SNE_Vx_Vy(Machine_t* machine, Instruction_t* instruction)
{
  // 9xy0 - SNE Vx, Vy
  // Skip next instruction if Vx != Vy.

  // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.

  uint8_t Vx = (*instruction)[0] & 0x0F;
  uint8_t Vy = (((*instruction)[1]) & 0xF0) >> 4;
  if(machine->REGISTERS[Vx] != machine->REGISTERS[Vy])
  {
    machine->PC += 2;
  }
}

void JP_V0(Machine_t* machine, Instruction_t* instruction)
{
  // Bnnn - JP V0, addr
  // Jump to location nnn + V0.

  // The program counter is set to nnn plus the value of V0.

  uint16_t nnn = (((*instruction)[0] & 0x0F) << 8) + (*instruction)[1];
  machine->PC = machine->REGISTERS[0x0] + nnn;
}

void RND_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Cxkk - RND Vx, byte
  // Set Vx = random byte AND kk.

  // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
  // The results are stored in Vx. See instruction 8xy2 for more information on AND.
  uint8_t random = rand();
  uint8_t Vx = (*instruction)[0] & 0xF;
  machine->REGISTERS[Vx] = random & (*instruction)[1];
}

void LD_Vx_DT(Machine_t* machine, Instruction_t* instruction)
{
  // Fx07 - LD Vx, DT
  // Set Vx = delay timer value.

  // The value of DT is placed into Vx.
  uint8_t Vx = (*instruction)[0] & 0x0F;
  machine->REGISTERS[Vx] = machine->DELAY_TIMER;
}

void LD_DT_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Fx15 - LD DT, Vx
  // Set delay timer = Vx.

  // DT is set equal to the value of Vx.

  uint8_t Vx = (*instruction)[0] & 0xF;
  machine->DELAY_TIMER = machine->REGISTERS[Vx];
}

void LD_ST_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Fx18 - LD ST, Vx
  // Set sound timer = Vx.

  // ST is set equal to the value of Vx.

  uint8_t Vx = (*instruction)[0] & 0xF;
  machine->SOUND_TIMER = machine->REGISTERS[Vx];
}

void ADD_I_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Fx1E - ADD I, Vx
  // Set I = I + Vx.

  // The values of I and Vx are added, and the results are stored in I.

  uint8_t Vx = (*instruction)[0] & 0xF;
  machine->I += machine->REGISTERS[Vx];
}

void LD_F_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Fx29 - LD F, Vx
  // Set I = location of sprite for digit Vx.

  // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.

  uint8_t Vx = (*instruction)[0] & 0x0F;
  machine->I = FONT_BASE + (machine->REGISTERS[Vx] & 0x0F);
}

void LD_B_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Fx33 - LD B, Vx
  // Store BCD representation of Vx in memory locations I, I+1, and I+2.

  // The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
  // the tens digit at location I+1, and the ones digit at location I+2.

  uint8_t Vx = (*instruction)[0] & 0x0F;
  uint8_t val = machine->REGISTERS[Vx];

  uint8_t ones = val % 10;
  uint8_t tens = ((val % 100) - ones) / 10;
  uint8_t hundreds = (val - (tens + ones)) / 100;

  machine->MEMORY[machine->I] = hundreds;
  machine->MEMORY[machine->I + 1] = tens;
  machine->MEMORY[machine->I + 2] = ones;
}

void LD_I_Vx(Machine_t* machine, Instruction_t* instruction)
{
  // Fx55 - LD [I], Vx
  // Store registers V0 through Vx in memory starting at location I.

  // The interpreter copies the values of registers V0 through Vx into memory,
  // starting at the address in I.
  uint8_t i = 0;
  uint8_t Vx = (*instruction)[0] & 0x0F;
  for(; i <= Vx; ++i)
  {
    machine->MEMORY[machine->I + i] = machine->REGISTERS[i];
  }
}

void LD_Vx_I(Machine_t* machine, Instruction_t* instruction)
{
  // Fx65 - LD Vx, [I]
  // Read registers V0 through Vx from memory starting at location I.

  // The interpreter reads values from memory starting at location I into registers V0 through Vx.

  uint8_t i = 0;
  uint8_t Vx = (*instruction)[0] & 0xF;

  for(; i <= Vx; ++i)
  {
    machine->REGISTERS[i] = machine->MEMORY[machine->I + i];
  }
}

void decode_0xF(Machine_t* machine, Instruction_t* instruction)
{
  switch((*instruction)[1])
  {
    case 0x07:
      LD_Vx_DT(machine, instruction);
      break;
    case 0x15:
      LD_DT_Vx(machine, instruction);
      break;
    case 0x18:
      LD_ST_Vx(machine, instruction);
      break;
    case 0x1E:
      ADD_I_Vx(machine, instruction);
      break;
    case 0x29:
      LD_F_Vx(machine, instruction);
      break;
    case 0x33:
      LD_B_Vx(machine, instruction);
      break;
    case 0x55:
      LD_I_Vx(machine, instruction);
      break;
    case 0x65:
      LD_Vx_I(machine, instruction);
      break;
  }
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
      break;
    case 0x3:
      SE_Vx(machine, instruction);
      break;
    case 0x4:
      SNE_Vx(machine, instruction);
      break;
    case 0x5:
      SE_Vx_Vy(machine, instruction);
      break;
    case 0x6:
      LD_Vx(machine, instruction);
      break;
    case 0x7:
      ADD_Vx(machine, instruction);
      break;
    case 0x8:
      decode_ALU(machine, instruction);
      break;
    case 0x9:
      SNE_Vx_Vy(machine, instruction);
      break;
    case 0xA:
      LD_I(machine, instruction);
      break;
    case 0xB:
      JP_V0(machine, instruction);
      break;
    case 0xC:
      RND_Vx(machine, instruction);
      break;
    case 0xD:
      DRW_VX_VY(machine, instruction);
      break;
    case 0xF:
      decode_0xF(machine, instruction);
      break;
  }

}

// INSTRUCTIONS:
// X --- 00E0 - CLS
// X --- 00EE - RET
// X --- 0nnn - SYS addr
// X --- 1nnn - JP addr
// X --- 2nnn - CALL addr
// X --- 3xkk - SE Vx, byte
// X --- 4xkk - SNE Vx, byte
// X --- 5xy0 - SE Vx, Vy
// X --- 6xkk - LD Vx, byte
// X --- 7xkk - ADD Vx, byte
// X --- 8xy0 - LD Vx, Vy
// X --- 8xy1 - OR Vx, Vy
// X --- 8xy2 - AND Vx, Vy
// X --- 8xy3 - XOR Vx, Vy
// X --- 8xy4 - ADD Vx, Vy
// X --- 8xy5 - SUB Vx, Vy
// X --- 8xy6 - SHR Vx {, Vy}
// X --- 8xy7 - SUBN Vx, Vy
// X --- 8xyE - SHL Vx {, Vy}
// X --- 9xy0 - SNE Vx, Vy
// X --- Annn - LD I, addr
// X --- Bnnn - JP V0, addr
// X --- Cxkk - RND Vx, byte
// X --- Dxyn - DRW Vx, Vy, nibble
//
// TODO:
// O --- Ex9E - SKP Vx
// O --- ExA1 - SKNP Vx
//
// X --- Fx07 - LD Vx, DT
// O --- Fx0A - LD Vx, K
// X --- Fx15 - LD DT, Vx
// X --- Fx18 - LD ST, Vx
// X --- Fx1E - ADD I, Vx
// X --- Fx29 - LD F, Vx
// X --- Fx33 - LD B, Vx
// X --- Fx55 - LD [I], Vx
// X --- Fx65 - LD Vx, [I]