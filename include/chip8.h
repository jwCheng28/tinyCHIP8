#ifndef CHIP8_H
#define CHIP8_h

#define MEMSIZE 4096
#define DISPWIDTH 64
#define DISPHEIGHT 32
#define DISPSIZE DISPWIDTH * DISPHEIGHT
#define PROGSTART 0x0200
#define FONTSETSTART 0x0050
#define FONTSETSIZE 80

#define ERR_PC_OUT_OF_RANGE -1

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
    uint8_t draw_f;
} Chip8;

void debug_print(Chip8* chip);

void init_chip(Chip8* chip);
Chip8* create_chip();
void delete_chip(Chip8* chip);
void load_program(Chip8* chip, FILE* program);
int8_t cycle(Chip8* chip);

#endif
