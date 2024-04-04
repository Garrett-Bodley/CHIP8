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