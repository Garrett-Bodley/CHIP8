#include "../headers/instruction.h"
#include "../headers/machine.h"
#include <stdio.h>

void JP(Instruction_t* instruction, Machine_t* machine)
{
  // 1nnn - JP addr
  // Jump to location nnn.

  // The interpreter sets the program counter to nnn.
  machine->PC = (*instruction)[0] & 0x0F;
  machine->PC <<= 8;
  machine->PC |= (*instruction)[1];
}

void LD_Vx(Instruction_t* instruction, Machine_t* machine)
{
  // Set Vx = kk.

  // The interpreter puts the value kk into register Vx.
  (*instruction)[0] &= 0x0F;
  machine->REGISTERS[(*instruction)[0]] = (*instruction)[1];
}

void ADD_Vx(Instruction_t* instruction, Machine_t* machine)
{
  // 7xkk - ADD Vx, byte
  // Set Vx = Vx + kk.

  // Adds the value kk to the value of register Vx, then stores the result in Vx.
  (*instruction)[0] &= 0x0F;
  machine->REGISTERS[(*instruction[0])] += (*instruction)[1];
}

void LD_I(Instruction_t*instruction, Machine_t* machine)
{
  // Annn - LD I, addr
  // Set I = nnn.

  // The value of register I is set to nnn.
  (*instruction)[0] &= 0x0F;
  machine->I = (*instruction)[0];
  machine->I <<= 8;
  machine->I |= (*instruction)[1];
}

void decode(Instruction_t* instruction, Machine_t* machine)
{
  uint8_t first_nibble = *instruction[0] >> 4;
  switch(first_nibble)
  {
    case 0x1:
      JP(instruction, machine);
      break;
    case 0x6:
      LD_Vx(instruction, machine);
      break;
    case 0x7:
      ADD_Vx(instruction, machine);
      break;
    case 0xA:
      LD_I(instruction, machine);
      break;
  }

}