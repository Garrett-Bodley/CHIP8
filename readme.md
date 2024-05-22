# CHIP-8

I made a CHIP-8 virtual machine in C!

This is a cross-platform implementation, meaning that it can compile to both run on your PC as well as cross-compile to run on an Apple //e. It expects the //e to have an 80-column card.

## What is CHIP-8?

> CHIP-8 is an interpreted programming language, developed by Joseph Weisbecker made on his 1802 Microprocessor. It was initially used on the COSMAC VIP and Telmac 1800 8-bit microcomputers in the mid-1970s. CHIP-8 programs are run on a CHIP-8 virtual machine. — [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)

## Cross Compilation

Compiling to the //e target is accomplished via the use of the [CC65](https://cc65.github.io/) tool chain, a cross development package for 6502 systems.

## Apple //e

The Apple //e is the third model in the Apple II series of personal computers produced by Apple Computer. It runs on a MOS-6502 processor, the mighty 8-bit processor that helped sparked the home computing revolution of the early 1980s.

The //e's screen memory buffers are rather [peculiar](https://en.wikipedia.org/wiki/Apple_II_graphics#Peculiarity_of_graphics_modes). I make use of the lesser-documented **80-column Low Resolution Graphics** mode which is not without its difficulties. Even screen columns are on **Page 2** of screen memory, while odd columns are on **Page 1**. Switching between memory pages is accomplished by writing to the correct "soft switch".

Meanwhile, even rows are written to the "low-nibble" (the 4 least significant bits) of the corresponding address and odd rows written to the "high-nibble". All of this means that the same memory address may correspond to one of four possible pixels. Special care must be taken to ensure the desired pixel is turned on.

## Dependencies

```
brew install criterion sdl2 cc65 c2t
```

## Build

### Non Apple //e build:

```
make sdl
./bin/sdl/chip8 Path/to/a/valid/CHIP8/ROM/file.ch8
```

### Apple //e build:
```
# Compiles code into an executable 6502 file
make apple

# Generates a .wav file for analog audio transfer to a real-world //e
make wav

# Generates a .dsk file for use with Virtual ][ or other //e emulators
make disk
```

## TODO:

- [ ] Get keyboard input working on SDL build
- [ ] Get audio audio output working on the SDL build
- [ ] Rewrite //e implementation's draw screen routine in assembly (current C code is slower than desired)
- [ ] Incorporate `load_rom.rb` into the Makefile build processes
- [ ] Make `instruction` and `machine` extern values