#include <SDL.h>
#include <criterion/criterion.h>
#include "../../headers/machine.h"
#include "../../headers/instruction.h"
#include "../../headers/decode.h"

extern Machine_t machine;
extern Instruction_t instruction;

// These tests cover all instructions needed to run the IBM ROM

void setup(void)
{
  sys_init(&machine);
}

Test(IBM, JP)
{
  // 1nnn - JP addr
  // Jump to location nnn.

  // The interpreter sets the program counter to nnn.

  instruction[0] = 0x1F;
  instruction[1] = 0xFF;
  decode(&machine, &instruction);
  cr_assert(machine.PC == 0xFFF, "Expected machine PC to be 0xFFF, got %03X.", machine.PC);
}

Test(IBM, LD_Vx)
{
  // Set Vx = kk.

  // The interpreter puts the value kk into register Vx.

  instruction[0] = 0x60;
  instruction[1] = 0x12;

  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0x0] == 0x12, "Expected register 0x0 to have value of 0x12, found %02x", machine.REGISTERS[0x0]);

  instruction[0] = 0x61;
  instruction[1] = 0x34;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0x0] == 0x12, "Expected register 0x0 to have value of 0x12, found %02x", machine.REGISTERS[0x0]);
  cr_expect(machine.REGISTERS[0x1] == 0x34, "Expected register 0x1 to have value of 0x34, found %02x", machine.REGISTERS[0x1]);

  instruction[0] = 0x62;
  instruction[1] = 0x56;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0x0] == 0x12, "Expected register 0x0 to have value of 0x12, found %02x", machine.REGISTERS[0x0]);
  cr_expect(machine.REGISTERS[0x1] == 0x34, "Expected register 0x1 to have value of 0x34, found %02x", machine.REGISTERS[0x1]);
  cr_expect(machine.REGISTERS[0x2] == 0x56, "Expected register 0x2 to have value of 0x56, found %02x", machine.REGISTERS[0x2]);

  instruction[0] = 0x60;
  instruction[1] = 0x78;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0x0] == 0x78, "Expected register 0x0 to have value of 0x78, found %02x", machine.REGISTERS[0x0]);
  cr_expect(machine.REGISTERS[0x1] == 0x34, "Expected register 0x1 to have value of 0x34, found %02x", machine.REGISTERS[0x1]);
  cr_expect(machine.REGISTERS[0x2] == 0x56, "Expected register 0x2 to have value of 0x56, found %02x", machine.REGISTERS[0x2]);
}

Test(IBM, ADD_Vx)
{
  // 7xkk - ADD Vx, byte
  // Set Vx = Vx + kk.

  // Adds the value kk to the value of register Vx, then stores the result in Vx.

  instruction[0] = 0x7A;
  instruction[1] = 0x12;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0xA] == 0x12, "Expected register 0xA to contain value 0x12, found %02X.", machine.REGISTERS[0xA]);
  instruction[0] = 0x7A;
  instruction[1] = 0x11;
  decode(&machine, &instruction);
  cr_expect(machine.REGISTERS[0xA] == 0x23, "Expected register 0xA to contain value 0x23, found %02X.", machine.REGISTERS[0xA]);
}

Test(IBM, LD_I)
{
  // Annn - LD I, addr
  // Set I = nnn.

  // The value of register I is set to nnn.

  instruction[0] = 0xA1;
  instruction[1] = 0x23;
  decode(&machine, &instruction);
  cr_assert(machine.I == 0x123, "Expected register I to contain 0x123, found %03X.", machine.I);
}

Test(IBM, RET)
{
  // 00EE - RET
  // Return from a subroutine.

  // The interpreter sets the program counter to the address at the top of the stack,
  // then subtracts 1 from the stack pointer.

  instruction[0] = 0x00;
  instruction[1] = 0xEE;

  machine.STACK[0] = 0x123;
  machine.SP = 1;
  decode(&machine, &instruction);
  cr_expect(machine.SP == 0, "Expected SP to be 1, found %i", machine.SP);
  cr_expect(machine.PC == 0x123, "Expected PC to be 0x123, found %03X.", machine.PC);
}

Test(IBM, CLS)
{
  // 00E0 - CLS
  // Clear the display.

  // Init SDL Surface
  SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0 , 64, 32, 1, SDL_PIXELFORMAT_INDEX1MSB);
  machine.SCREEN = surface;
  // Fill all bytes with 0xFF
  memset(surface->pixels, 0xff, SCREEN_REGISTER_COUNT);

  // Decode CLS instruction
  instruction[0] = 0x00;
  instruction[1] = 0xE0;
  decode(&machine, &instruction);

  // Ensure Screen is clear
  uint8_t* pixels = (uint8_t*)machine.SCREEN->pixels;
  for(int i = 0; i < SCREEN_REGISTER_COUNT; i++)
  {
    cr_expect(pixels[i] == 0x00, "At index %i, expected 0x00, got 0x%02X", i, pixels[i]);
  }
}