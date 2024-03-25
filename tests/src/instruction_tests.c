#include <SDL.h>
#include <criterion/criterion.h>
#include "../../headers/machine.h"
#include "../../headers/instruction.h"
#include "../../headers/decode.h"

extern Machine_t machine;
extern Instruction_t instruction;

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

Test(Instruction, RET)
{
  instruction[0] = 0x00;
  instruction[1] = 0xEE;

  machine.STACK[0] = 0x123;
  machine.SP = 1;
  decode(&machine, &instruction);
  cr_expect(machine.SP == 0, "Expected SP to be 1, found %i", machine.SP);
  cr_expect(machine.PC == 0x123, "Expected PC to be 0x123, found %03X.", machine.PC);
}

Test(Instruction, CLS)
{
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
