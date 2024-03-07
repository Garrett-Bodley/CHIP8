#include <stdint.h>

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

struct Instruction_s {
  uint8_t first;
  uint8_t second;
  u_int8_t first_nibble;
};

typedef struct Instruction_s Instruction_t;

#endif