#include <SDL.h>
#include <criterion/criterion.h>
#include "../../headers/machine.h"
#include "../../headers/instruction.h"
#include "../../headers/decode.h"

extern Machine_t machine;
extern Instruction_t instruction;

// Tests related to ALU functionality

void setup(void)
{
  sys_init(&machine);
}

Test(Instruction, LD_Vx_Vy)
{
  // 8xy0 - LD Vx, Vy
  // Set Vx = Vy.

  // Stores the value of register Vy in register Vx.

  // Set REGISTERS[0x0] = REGISTERS[0x1]
  instruction[0] = 0x80;
  instruction[1] = 0x10;
  // Set REGISTERS[1] to contain the value 0xFF
  machine.REGISTERS[0x01] = 0xFF;

  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0x0] == 0xFF, "Expected register 0x0 to contain value 0xFF, found %02x", machine.REGISTERS[0x0]);

  // Set REGISTERS[0x3] = REGISTERS[0xA]
  instruction[0] = 0x83;
  instruction[1] = 0xA0;
  // Set REGISTERS[1] to contain the value 0xFF
  machine.REGISTERS[0xA] = 0x42;

  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0xA] == 0x42, "Expected register 0xA to contain value 0x42, found %02x", machine.REGISTERS[0x0]);
}