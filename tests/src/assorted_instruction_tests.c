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

Test(Assorted, CALL)
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

Test(Assorted, SE_Vx_Vy)
{
  // 5xy0 - SE Vx, Vy
  // Skip next instruction if Vx = Vy.

  // The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.

  instruction[0] = 0x50;
  instruction[1] = 0x10;
  machine.PC = 0x00;
  machine.REGISTERS[0x0] = 0x42;
  machine.REGISTERS[0x1] = 0x42;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x02, "Expected PC to be set to 0x02, got 0x%02x", machine.PC);

  instruction[0] = 0x5A;
  instruction[1] = 0xB0;
  machine.PC = 0x00;
  machine.REGISTERS[0xA] = 0x42;
  machine.REGISTERS[0xB] = 0x41;

  cr_expect(machine.PC == 0x00, "Expected PC to be set to 0x02, got 0x%02x", machine.PC);
}

Test(Assorted, SNE_Vx_Vy)
{
  // 9xy0 - SNE Vx, Vy
  // Skip next instruction if Vx != Vy.

  // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.

  instruction[0] = 0x90;
  instruction[1] = 0x10;
  machine.PC = 0x00;
  machine.REGISTERS[0x0] = 0x42;
  machine.REGISTERS[0x1] = 0x41;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x02, "Expected PC to be set to 0x02, found 0x%02x", machine.PC);

  instruction[0] = 0x9A;
  instruction[1] = 0xB0;
  machine.PC = 0x00;
  machine.REGISTERS[0xA] = 0x42;
  machine.REGISTERS[0xB] = 0x42;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x00, "Expected PC to be set to 0x00, found 0x%02x", machine.PC);
}

Test(Assorted, JP_V0)
{
  // Bnnn - JP V0, addr
  // Jump to location nnn + V0.

  // The program counter is set to nnn plus the value of V0.

  instruction[0] = 0xB1;
  instruction[1] = 0x11;
  machine.REGISTERS[0x0] = 0x11;
  machine.PC = 0x000;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x122, "Expected PC to be set to 0x222, found 0x%03x", machine.PC);

  instruction[0] = 0xB1;
  instruction[1] = 0x23;
  machine.REGISTERS[0x0] = 0x11;
  machine.PC = 0x000;

  decode(&machine, &instruction);

  cr_expect(machine.PC == 0x134, "Expected PC to be set to 0x134, found 0x%03x", machine.PC);

}

Test(Assorted, RND_Vx)
{
  // Cxkk - RND Vx, byte
  // Set Vx = random byte AND kk.

  // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
  // The results are stored in Vx. See instruction 8xy2 for more information on AND.

  // I can only sort of kind of test this one

  instruction[0] = 0xC0;
  instruction[1] = 0x34;

  decode(&machine, &instruction);

  cr_expect((machine.REGISTERS[0x0] & 0x34) <= 0x34, "Expected register V0 to contain a value <= 0x34, found 0x%02x", machine.REGISTERS[0x0]);

}

Test(Assorted, LD_Vx_DT)
{
  // Fx07 - LD Vx, DT
  // Set Vx = delay timer value.

  // The value of DT is placed into Vx.

  instruction[0] = 0xF0;
  instruction[1] = 0x07;
  machine.DELAY_TIMER = 0x42;

  decode(&machine, &instruction);

  cr_expect(machine.REGISTERS[0x0] == 0x42, "Expected register V0 to contain the value 0x42, found 0x%02x", machine.REGISTERS[0x0]);
}

Test(Assorted, LD_DT_Vx)
{
  // Fx15 - LD DT, Vx
  // Set delay timer = Vx.

  // DT is set equal to the value of Vx.

  instruction[0] = 0xF0;
  instruction[1] = 0x15;
  machine.REGISTERS[0x0] = 0xFF;
  machine.DELAY_TIMER = 0x00;

  decode(&machine, &instruction);

  cr_expect(machine.DELAY_TIMER == 0xFF, "Expected DELAY_TIMER to be set to 0xFF, found 0x%02x", machine.DELAY_TIMER);
}

Test(Assorted, LD_ST_Vx)
{
  // Fx18 - LD ST, Vx
  // Set sound timer = Vx.

  // ST is set equal to the value of Vx.

  instruction[0] = 0xF0;
  instruction[1] = 0x18;
  machine.REGISTERS[0x0] = 0xFF;
  machine.SOUND_TIMER = 0x00;

  decode(&machine, &instruction);

  cr_expect(machine.SOUND_TIMER == 0xFF, "Expected SOUND_TIMER to be set to 0xFF, found 0x%02x", machine.SOUND_TIMER);
}

Test(Assorted, ADD_I_Vx)
{
  // Fx1E - ADD I, Vx
  // Set I = I + Vx.

  // The values of I and Vx are added, and the results are stored in I.

  instruction[0] = 0xF0;
  instruction[1] = 0x1E;
  machine.I = 0x111;
  machine.REGISTERS[0x0] = 0x22;

  decode(&machine, &instruction);

  cr_expect(machine.I == 0x133, "Expected I to be set to 0x133, found 0x%03x", machine.I);
}

Test(Assorted, LD_F_Vx)
{
  // Fx29 - LD F, Vx
  // Set I = location of sprite for digit Vx.

  // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.

  instruction[0] = 0xF0;
  instruction[1] = 0x29;
  machine.REGISTERS[0x0] = 0x00;

  decode(&machine, &instruction);

  cr_expect(machine.I == FONT_BASE, "Expected I to be set to 0x%02x, found 0x%02x", FONT_BASE, machine.I);

}