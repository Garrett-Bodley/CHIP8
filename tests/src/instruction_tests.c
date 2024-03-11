#include <criterion/criterion.h>
#include "../../headers/machine.h"
#include "../../headers/instruction.h"
#include "../../headers/decode.h"

Machine_t machine;
Instruction_t instruction;

void setup(void)
{
  sys_init(&machine);
}

Test(Instruction, JP)
{
  instruction[0] = 0x1F;
  instruction[1] = 0xFF;
  decode(&machine, &instruction);
  cr_assert(machine.PC == 0xFFF, "Expected machine PC to be 0xFFF, got %03X.", machine.PC);
}

Test(Instruction, LD_Vx)
{
  instruction[0] = 0x6a;
  instruction[1] = 0x15;
  decode(&machine, &instruction);
  cr_assert(machine.REGISTERS[0xA] == 0x15, "Expected register 0xA to contain value 0x15, found %02X.", machine.REGISTERS[0xA]);
}

Test(Instruction, ADD_Vx)
{
  instruction[0] = 0x7A;
  instruction[1] = 0x12;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0xA] == 0x12, "Expected register 0xA to contain value 0x12, found %02X.", machine.REGISTERS[0xA]);
  instruction[0] = 0x7A;
  instruction[1] = 0x11;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0xA] == 0x23, "Expected register 0xA to contain value 0x23, found %02X.", machine.REGISTERS[0xA]);
}

Test(Instruction, LD_I)
{
  instruction[0] = 0xA1;
  instruction[1] = 0x23;
  decode(&machine, &instruction);
  cr_assert(machine.I == 0x123, "Expected register I to contain 0x123, found %03X.", machine.I);
}