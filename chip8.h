#ifndef CHIP8_H
#define CHIP8_h

#define MEMSIZE 4096
#define DISPWIDTH 64
#define DISPHEIGHT 32
#define DISPSIZE DISPWIDTH * DISPHEIGHT
#define PROGSTART 0x0200
#define FONTSETSTART 0x0050
#define FONTSETSIZE 80

typedef struct {
    uint8_t memory[MEMSIZE];
    uint32_t display[DISPSIZE];
    uint16_t pc;
    uint16_t index;
    uint16_t stack[16];
    uint16_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t registers[16];
    uint8_t keypad[16];
} Chip8;

uint16_t fetch(Chip8* chip);
void decode_and_exec(Chip8* chip, uint16_t opcode);
void init_chip(Chip8* chip);
void load_program(Chip8* chip, FILE* program);

#endif
