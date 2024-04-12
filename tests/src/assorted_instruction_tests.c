#include <SDL.h>
#include <criterion/criterion.h>
#include "../../headers/machine.h"
#include "../../headers/instruction.h"
#include "../../headers/decode.h"

extern Machine_t machine;
extern Instruction_t instruction;

// Other Instruction tests

void setup(void)
{
  sys_init(&machine);
}

Test(Assorted_Instruction, CALL)
{
  instruction[0] = 0x23;
  instruction[1] = 0x33;
  machine.PC = 0x123;

  int old_SP = machine.SP;
  int old_PC = machine.PC;

  decode(&machine, &instruction);

  cr_expect(machine.SP == old_SP + 1, "Expected SP to be incremented");
  cr_expect(machine.STACK[machine.SP - 1] == old_PC, "Expected STACK[SP - 1] to be old PC value");
  cr_expect(machine.PC == 0x333, "Expected PC to be set to new value 0x333, got 0x%03x", machine.PC);
}

Test(Assorted, SE_Vx)
{
  // 3xkk - SE Vx, byte
  // Skip next instruction if Vx = kk.

  // The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.

  instruction[0] = 0x30;
  instruction[1] = 0xFF;
  machine.REGISTERS[0x0] = 0xFF;
  machine.PC = 0x00;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x02, "Expected PC to be 0x02, found %02x", machine.PC);

  instruction[0] = 0x3A;
  instruction[1] = 0x42;
  machine.REGISTERS[0xA] = 0x42;
  machine.PC = 0x04;
  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x06, "Expected PC to be 0x06, found %02x", machine.PC);

  instruction[0] = 0x3B;
  instruction[1] = 0x42;
  machine.REGISTERS[0xB] = 0x00;
  machine.PC = 0x00;

  cr_expect(machine.PC == 0x00, "Expected PC to be 0x00, found %02x", machine.PC);
}

Test(Assorted, SNE_Vx)
{
  // 4xkk - SNE Vx, byte
  // Skip next instruction if Vx != kk.

  // The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.

  instruction[0] = 0x40;
  instruction[1] = 0xFF;
  machine.PC = 0x00;
  machine.REGISTERS[0x0] = 0x42;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x02, "Expected PC to be set to 0x02, found 0x%02x", machine.PC);

  instruction[0] = 0x41;
  instruction[1] = 0xFF;
  machine.PC = 0x00;
  machine.REGISTERS[0x1] = 0xFF;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x00, "Expected PC to be set to 0x00, found 0x%02x", machine.PC);
}

