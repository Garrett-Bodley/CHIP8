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
  decode(&instruction, &machine);
  cr_assert(machine.PC == 0xFFF, "Expected machine PC to be 0xFFF, got %03X.", machine.PC);
}