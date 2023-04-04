# tinyCHIP8

This project is an attempt to create an emulator for [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8).

## Current Progress

- Most instructions are implemented except 0xFX-- & 0xCXNN
- Able successfully run and display binary program that doesn't require those unimplemented instructions

## How to run
This code currently only supports windows.
### Dependencies
- mingw32 compiler
- SDL2 library

Given these requirements are met, code could be compile by running `mingw32-make` in the terminal.

Then, a CHIP-8 program binary could runned using `emu.exe [insert program name]` in the terminal.

## Reference Documentation
[CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#memmap)

[Mastering CHIP‐8](https://github.com/mattmikolay/chip-8/wiki/Mastering-CHIP%E2%80%908)
