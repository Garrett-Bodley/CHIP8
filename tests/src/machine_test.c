#include <criterion/criterion.h>
#include "../../headers/machine.h"
#include "../../headers/instruction.h"
#include <stddef.h>

Machine_t machine;
Instruction_t instruction;

void log_memory_test()
{
  for(int i = 0; i < MEM_SIZE; i++)
  {
    if(i % 16 == 0 && i > 0){ printf("\n"); }
    if(i % 16 == 0){ printf("%04x  ", i); }
    printf("%02x " , machine.MEMORY[i]);
    if(i % 16 == 7){ printf(" "); }
  }
  printf("\n");
}

Test(Machine, Clear)
{
  for(int i = 0; i < REGISTER_COUNT; i++)
  {
    machine.REGISTERS[i] = 0xff;
  }

  for(int i = 0; i < MEM_SIZE; i++)
  {
    machine.MEMORY[i] = 0xff;
  }

  clear_machine(&machine);

  uint8_t* machine_bytes = (uint8_t*)&machine;
  for(int i = 0; i < sizeof(Machine_t); i++)
  {
    cr_assert(machine_bytes[i] == 0, "Machine byte %d is not zero as expected.", i);
  }
}

Test(Machine, Init)
{
  sys_init(&machine);
  cr_expect(machine.PC == 0x200, "Expected machine PC to be 0x200, got %03X.", machine.PC);
}

Test(Machine, Load_File)
{
  clear_machine();

  size_t rom_size = 132;
  uint8_t IBM_ROM[] = {
    0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09, 0xa2, 0x39, 0xd0, 0x1f,
    0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04, 0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66,
    0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x75, 0xd0, 0x1f, 0x12, 0x28, 0xff, 0x00, 0xff, 0x00, 0x3c, 0x00,
    0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x38, 0x00, 0x3f,
    0x00, 0x3f, 0x00, 0x38, 0x00, 0xff, 0x00, 0xff, 0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00,
    0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0xf8, 0x00, 0xfc, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x3b,
    0x00, 0x39, 0x00, 0xf8, 0x00, 0xf8, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0xbf, 0x00, 0xfb, 0x00,
    0xf3, 0x00, 0xe3, 0x00, 0x43, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
    0x00, 0xe0, 0x00, 0xe0
  };
  load_file("./ROMs/IBM.ch8");

  uint8_t* mem_bytes = (uint8_t*)&machine.MEMORY;
  for(int i = 0; i < rom_size; i++){
    cr_expect(mem_bytes[i + 0x200] == IBM_ROM[i], "At index %i, expected 0x%02X, got 0x%02X", i, IBM_ROM[i], mem_bytes[i]);
  };
  // Use this for debugging:
  // log_memory_test(&machine);
}

Test(Machine, Load_Font)
{
  clear_machine();
  load_font();

    uint8_t FONT[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  uint8_t* mem_bytes = (uint8_t*)&machine.MEMORY;
  uint8_t base_addr = 0x50;
  for(int i = 0; i < 80; i++)
  {
    cr_expect(mem_bytes[base_addr + i] == FONT[i],  "At index %i, expected 0x%02X, got 0x%02X", i, FONT[i], mem_bytes[i]);
  }
}

Test(Machine, Fetch)
{
  sys_init();
  machine.MEMORY[0x200] = 0xAB;
  machine.MEMORY[0x201] = 0xCD;

  fetch();
  cr_expect(instruction[0] == 0xAB, "Expected the first byte of the fetched instruction to be 0xAB, got 0x%02X instead.", instruction[0]);
  cr_expect(instruction[1] == 0xCD, "Expected the second byte of the fetched instruction to be 0xCD, got 0x%02X instead.", instruction[1]);
  cr_expect(machine.PC == 0x202, "Expected SP to be 0x202, got %30X instead.", machine.SP);
}

Test(Machine, SP_Fetch)
{
  sys_init();
  machine.MEMORY[0x300] = 0xAB;
  machine.MEMORY[0x301] = 0xCD;
  machine.PC = 0x300;

  fetch();
  cr_expect(instruction[0] == 0xAB, "Expected the first byte of the fetched instruction to be 0xAB, got 0x%02X instead.", instruction[0]);
  cr_expect(instruction[1] == 0xCD, "Expected the second byte of the fetched instruction to be 0xCD, got 0x%02X instead.", instruction[1]);
  cr_expect(machine.PC == 0x302, "Expected SP to be 0x202, got %30X instead.", machine.SP);
}

Test(Machine, ROM_Fetch)
{
  sys_init();
  size_t rom_size = 132;
  uint8_t IBM_ROM[] = {
    0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09, 0xa2, 0x39, 0xd0, 0x1f,
    0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04, 0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66,
    0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x75, 0xd0, 0x1f, 0x12, 0x28, 0xff, 0x00, 0xff, 0x00, 0x3c, 0x00,
    0x3c, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x38, 0x00, 0x3f,
    0x00, 0x3f, 0x00, 0x38, 0x00, 0xff, 0x00, 0xff, 0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00,
    0x80, 0x00, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0xf8, 0x00, 0xfc, 0x00, 0x3e, 0x00, 0x3f, 0x00, 0x3b,
    0x00, 0x39, 0x00, 0xf8, 0x00, 0xf8, 0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0xbf, 0x00, 0xfb, 0x00,
    0xf3, 0x00, 0xe3, 0x00, 0x43, 0xe0, 0x00, 0xe0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
    0x00, 0xe0, 0x00, 0xe0
  };
  load_file("./ROMs/IBM.ch8");

  for(int i = 0; i < rom_size; i += 2)
  {
    fetch();
    cr_expect(instruction[0] == IBM_ROM[i] && instruction[1] == IBM_ROM[i + 1], "ROM fetch error at byte %d: expected 0x%02X%02X, got 0x%02X%02X.", i, IBM_ROM[i], IBM_ROM[i + 1], instruction[0], instruction[1]);
  }
}