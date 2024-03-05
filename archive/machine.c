#include <stdint.h>

// #define APPLEII
#define MEM_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8

struct Machine_s {
  uint8_t SP;
  uint16_t I;
  uint16_t PC;
  uint8_t REGISTERS[REGISTER_COUNT];
  uint16_t STACK[STACK_SIZE];

  uint8_t DELAY_TIMER;
  uint8_t SOUND_TIMER;

  uint8_t MEMORY[MEM_SIZE];
};

typedef struct Machine_s Machine_t;

struct Machine_s my_machine;

union MEMORY {
  uint8_t main[MEM_SIZE];
  struct PROGRAM {
    uint8_t _unused[0x200];
    uint8_t program[MEM_SIZE - 0x200];
  } program_struct;
 };

#ifdef APPLEII
union MEMORY mem = { .program_struct.program = { 0x00, 0x01 } };
// #else
// union Memory mem;
#endif

