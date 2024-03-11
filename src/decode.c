#include "../headers/instruction.h"
#include "../headers/machine.h"
#include <stdio.h>

void JP(Machine_t* machine, Instruction_t* instruction)
{
  // 1nnn - JP addr
  // Jump to location nnn.

  // The interpreter sets the program counter to nnn.
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
  (*instruction)[0] &= 0x0F;
  machine->REGISTERS[(*instruction[0])] += (*instruction)[1];
}

void LD_I(Machine_t* machine, Instruction_t* instruction)
{
  // Annn - LD I, addr
  // Set I = nnn.

  // The value of register I is set to nnn.
  (*instruction)[0] &= 0x0F;
  machine->I = (*instruction)[0];
  machine->I <<= 8;
  machine->I |= (*instruction)[1];
}

void CLS(Machine_t* machine)
{
  // 00E0 - CLS
  // Clear the display.
  memset(machine->SCREEN->pixels, 0x00, SCREEN_REGISTER_COUNT);
}

void RET(Machine_t* machine)
{
  // 00EE - RET
  // Return from a subroutine.

  // The interpreter sets the program counter to the address at the top of the stack,
  // then subtracts 1 from the stack pointer.
  machine->PC = machine->STACK[machine->SP - 1];
  machine->SP -= 1;
}

void decode_0x0(Machine_t* machine, Instruction_t* instruction)
{
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

  uint8_t Vx = (*instruction)[0] & 0x0F;
  uint8_t Vy = ((*instruction)[1] & 0xF0) >> 4;
  uint8_t N = (*instruction)[1] & 0x0F;

  uint8_t x = machine->REGISTERS[Vx] & 63;
  uint8_t y = machine->REGISTERS[Vy] & 31;

  // set VF to 0
  machine->REGISTERS[0xF] = 0;

  int x_mem_offset = x / 8; // 8 pixels per word
  int shift_offset = x & 7; // x % 7
  for(int i = 0; i < N; i++)
  {
    uint8_t y_mem_offset = (y + i) * 64 / 8;
    uint8_t mem_offset = x_mem_offset + y_mem_offset;
    uint8_t sprite_word = machine->MEMORY[machine->I + i];

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
}

void decode(Machine_t* machine, Instruction_t* instruction)
{
  uint8_t first_nibble = (*instruction)[0] >> 4;
  switch(first_nibble)
  {
    case 0x0:
      decode_0x0(machine, instruction);
      break;
    case 0x1:
      JP(machine, instruction);
      break;
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