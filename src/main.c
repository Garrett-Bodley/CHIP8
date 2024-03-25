#include <stdio.h>
#include <stdbool.h>
#include "../headers/chip8.h"

int main(int argc, char* argv[])
{
  #ifdef SDL

  if(argc < 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }
  chip8(argv[1]);

  #elif defined(APPLE2)

  chip8(NULL);
  while(true);

  #endif

  return 0;
}