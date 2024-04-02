#include "instruction.h"

#ifndef DECODE_H
#define DECODE_H

void decode(Machine_t* machine, Instruction_t* instruction);

#ifdef APPLE2

void CLS(Machine_t* machine);

#endif

#endif