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

Test(ALU, LD_Vx_Vy)
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

Test(ALU, OR_Vx_Vy)
{
  // 8xy1 - OR Vx, Vy
  // Set Vx = Vx OR Vy.

  // Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A bitwise OR compares the
  // corrseponding bits from two values, and if either bit is 1, then the same bit in the result is also 1.
  // Otherwise, it is 0.

  instruction[0] = 0x80;
  instruction[1] = 0x11;
  machine.REGISTERS[0x0] = 0x00;
  machine.REGISTERS[0x1] = 0x01;
  decode(&machine, &instruction);

  cr_expect(machine.REGISTERS[0x0] == 0x01, "Expected REGSISTERS[0x0] to contain value 0x01, found %02x", machine.REGISTERS[0x0]);

  instruction[0] = 0x8A;
  instruction[1] = 0xB1;
  machine.REGISTERS[0xA] = 0x02;
  machine.REGISTERS[0xB] = 0x01;

  cr_expect(machine.REGISTERS[0xA] == 0x03, "Expected REGISTERS[0xA] to contain value 0x03, found %02x", machine.REGISTERS[0xA]);

  instruction[0] = 0x85;
  instruction[1] = 0x61;
  machine.REGISTERS[0x5] = 0x06;
  machine.REGISTERS[0x6] = 0x02;

  cr_expect(machine.REGISTERS[0x5] == 0x06, "Expected REGISTERS[0x5] to contain value 0x06, found %02x", machine.REGISTERS[0xA]);

}