# CHIP-8

I made a CHIP-8 virtual machine in C!

### What is CHIP-8?

> CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker made on his 1802 Microprocessor. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. — [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

## Build locally

```
brew install criterion
brew install sdl2
make
./bin/chip8 Path/to/a/valid/chip8/ROM/rom.ch8
```