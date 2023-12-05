#include <stdlib.h>

#define MEM_SIZE 4096
#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define SCREEN_REGISTER_COUNT 256 // 64 * 32 / 8

uint8_t SP;
uint16_t I;
uint8_t REGISTERS[REGISTER_COUNT];
uint16_t STACK[STACK_SIZE];

uint8_t DELAY_TIMER;
uint8_t SOUND_TIMER;

uint8_t MEMORY[MEM_SIZE];
uint8_t SCREEN[SCREEN_REGISTER_COUNT];

int main()
{

}