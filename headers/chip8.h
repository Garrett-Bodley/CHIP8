#ifndef CHIP8_H
#define CHIP8_H

#ifdef SDL
void chip8(char* path);
#elif defined(APPLE2)
void chip8();
#endif



#endif