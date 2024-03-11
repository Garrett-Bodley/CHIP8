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

void decode(Machine_t* machine, Instruction_t* instruction)
{
  uint8_t first_nibble = *instruction[0] >> 4;
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
  }

}