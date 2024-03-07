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
  }

}