#include <stdio.h>
#include "../headers/chip8.h"

int main(int argc, char* argv[])
{
  if(argc < 2)
  {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  chip8(argv[1]);
  return 0;
}