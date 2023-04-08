#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"

uint8_t fontset[FONTSETSIZE] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,
    0x90, 0x90, 0xf0, 0x10, 0x10,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,
    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0xf0, 0x10, 0x20, 0x40, 0x40,
    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,
    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,
    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,
    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80
};

void debug_print(Chip8* chip) {
    printf("PC: %d\t", chip->pc);
    printf("SP: %d\t", chip->sp);
    printf("I: %d\n", chip->index);

    for (uint8_t i = 0; i < 16; ++i) printf("k[%d]:%d ", i, chip->keypad[i]);
    printf("\n");

    for (uint8_t i = 0; i < 16; ++i) printf("r[%d]:%d ", i, chip->registers[i]);
    printf("\n");
    printf("\n");
}

void ret(Chip8* chip) {
    chip->sp--;
    chip->pc = chip->stack[chip->sp];
}

void jump(Chip8* chip, uint16_t opcode) {
    chip->pc = opcode & 0x0FFF;
}

void call(Chip8* chip, uint16_t opcode) {
    chip->stack[chip->sp] = chip->pc;
    chip->sp++;
    chip->pc = opcode & 0x0FFF;
}

void skip_if_eq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    if (rx == (opcode & 0x00FF)) chip->pc += 2;
}

void skip_if_neq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    if (rx != (opcode & 0x00FF)) chip->pc += 2;
}

void skip_if_reg_eq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    uint8_t ry = chip->registers[(opcode & 0x00F0) >> 4];
    if (rx == ry) chip->pc += 2;
}

void store_imd(Chip8* chip, uint16_t opcode) {
    chip->registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
}

void add_imd(Chip8* chip, uint16_t opcode) {
    chip->registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
}

void arithmetic(Chip8* chip, uint16_t opcode) {
    uint8_t xid = (opcode & 0x0F00) >> 8;
    uint8_t yid = (opcode & 0x00F0) >> 4;
    uint8_t rx = chip->registers[xid];
    uint8_t ry = chip->registers[yid];
    if ((opcode & 0x000F) == 0x0000) {
        chip->registers[xid] = ry;
    } else if ((opcode & 0x000F) == 0x0001) {
        chip->registers[xid] =  rx | ry;
    } else if ((opcode & 0x000F) == 0x0002) {
        chip->registers[xid] =  rx & ry;
    } else if ((opcode & 0x000F) == 0x0003) {
        chip->registers[xid] =  rx ^ ry;
    } else if ((opcode & 0x000F) == 0x0004) {
        uint16_t sum = rx + ry;
        chip->registers[0xF] = !!(sum & 0x0F00);
        chip->registers[xid] = sum & 0x00FF;
    } else if ((opcode & 0x000F) == 0x0005) {
        chip->registers[0xF] = !!(rx > ry);
        chip->registers[xid] -= ry;
    } else if ((opcode & 0x000F) == 0x0006) {
        chip->registers[0xF] = rx & 0x01;
        chip->registers[xid] >>= 1;
    } else if ((opcode & 0x000F) == 0x0007) {
        chip->registers[0xF] = !!(ry > rx);
        chip->registers[xid] = ry - rx;
    } else if ((opcode & 0x000F) == 0x000E) {
        chip->registers[0xF] = (rx & 0x80) >> 7;
        chip->registers[xid] <<= 1;
    } else {
        printf("Program Error\nQuiting...\n");
        exit(0);
    }
}

void skip_if_reg_neq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    uint8_t ry = chip->registers[(opcode & 0x00F0) >> 4];
    if (rx != ry) chip->pc += 2;
}

void index_store(Chip8* chip, uint16_t opcode) {
    chip->index = opcode & 0x0FFF;
}

void relative_jump(Chip8* chip, uint16_t opcode) {
    chip->pc = (opcode & 0x0FFF) + chip->registers[0];
}

void rand_and(Chip8* chip, uint16_t opcode) {
    uint8_t mask = opcode & 0x00FF;
    chip->registers[(opcode & 0x0F00) >> 8] = (rand() % 256) & mask;
}

void draw(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8] % DISPWIDTH;
    uint8_t ry = chip->registers[(opcode & 0x00F0) >> 4] % DISPHEIGHT;
    uint8_t nsize = opcode & 0x000F;
    uint16_t disp_index;
    uint8_t byte, row, col;
    chip->registers[0xF] = 0;
    for (uint8_t i = 0; i < nsize; ++i) {
        byte = chip->memory[chip->index + i];
        row = (ry + i) % DISPHEIGHT;
        for (uint8_t j = 0; j < 8; ++j) {
            col = (rx + j) % DISPWIDTH;
            disp_index = row*DISPWIDTH + col;
            if (chip->display[disp_index] && (byte & 0x80)) chip->registers[0xF] = 1;
            chip->display[disp_index] ^= (!!(byte & 0x80) * 0xFFFFFFFF);
            byte <<= 1;
        }
    }
    chip->draw_f = 0xFF;
}

void skip_if_key(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8] % 16;
    uint8_t key = chip->keypad[rx];
    if ( ((opcode & 0x00FF) == 0x9E && key) || ((opcode & 0x00FF) == 0xA1 && !key) ) chip->pc += 2; 
}

void set_regx_to_delay(Chip8* chip, uint16_t opcode) {
    chip->registers[(opcode & 0x0F00) >> 8] = chip->delay_timer;
}

int8_t key_press(Chip8* chip) {
    if (chip->keypad[0]) return 0;
    if (chip->keypad[1]) return 1;
    if (chip->keypad[2]) return 2;
    if (chip->keypad[3]) return 3;
    if (chip->keypad[4]) return 4;
    if (chip->keypad[5]) return 5;
    if (chip->keypad[6]) return 6;
    if (chip->keypad[7]) return 7;
    if (chip->keypad[8]) return 8;
    if (chip->keypad[9]) return 9;
    if (chip->keypad[10]) return 10;
    if (chip->keypad[11]) return 11;
    if (chip->keypad[12]) return 12;
    if (chip->keypad[13]) return 13;
    if (chip->keypad[14]) return 14;
    if (chip->keypad[15]) return 15;
    chip->pc -= 2;
    return -1;
}

void set_regx_to_key_press(Chip8* chip, uint16_t opcode) {
    int8_t key = key_press(chip);
    if (key < 0) return;
    chip->registers[(opcode & 0x0F00) >> 8] = key;
}

void set_delay_to_regx(Chip8* chip, uint16_t opcode) {
    chip->delay_timer = chip->registers[(opcode & 0x0F00) >> 8];
}

void set_sound_to_regx(Chip8* chip, uint16_t opcode) {
    chip->sound_timer = chip->registers[(opcode & 0x0F00) >> 8];
}

void add_regx_to_index(Chip8* chip, uint16_t opcode) {
    chip->index += chip->registers[(opcode & 0x0F00) >> 8];
}

void set_index_to_regx(Chip8* chip, uint16_t opcode) {
    chip->index = FONTSETSTART + 5 * chip->registers[(opcode & 0x0F00) >> 8];
}

void store_bcd_regx_in_mem(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    for (int8_t i = 2; i >= 0; --i) {
        chip->memory[chip->index + i] = rx % 10;
        rx /= 10;
    }
}

void store_reg0_to_regx_in_mem(Chip8* chip, uint16_t opcode) {
    uint8_t xid = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= xid; ++i) {
        chip->memory[chip->index + i] = chip->registers[i];
    }
}

void load_mem_to_reg0_to_regx(Chip8* chip, uint16_t opcode) {
    uint8_t xid = (opcode & 0x0F00) >> 8;
    for (uint8_t i = 0; i <= xid; ++i) {
        chip->registers[i] = chip->memory[chip->index + i];
    }
}

void ftype(Chip8* chip, uint16_t opcode) {
    uint8_t func_code = opcode & 0x00FF;
    if (func_code == 0x07) set_regx_to_delay(chip, opcode);
    if (func_code == 0x0A) set_regx_to_key_press(chip, opcode);
    if (func_code == 0x15) set_delay_to_regx(chip, opcode);
    if (func_code == 0x18) set_sound_to_regx(chip, opcode);
    if (func_code == 0x1E) add_regx_to_index(chip, opcode);
    if (func_code == 0x29) set_index_to_regx(chip, opcode);
    if (func_code == 0x33) store_bcd_regx_in_mem(chip, opcode);
    if (func_code == 0x55) store_reg0_to_regx_in_mem(chip, opcode);
    if (func_code == 0x65) load_mem_to_reg0_to_regx(chip, opcode);
}

uint16_t fetch(Chip8* chip) {
    return (chip->memory[chip->pc++] << 8) + chip->memory[chip->pc++];
}

void decode_and_exec(Chip8* chip, uint16_t opcode) {
    if ((opcode & 0xFF0F) == 0x0000) memset(chip->display, 0, DISPSIZE * 4);
    else if ((opcode & 0xFF00) == 0x0000) ret(chip);
    else if ((opcode & 0xF000) == 0x0000) {}
    else if ((opcode & 0xF000) == 0x1000) jump(chip, opcode);
    else if ((opcode & 0xF000) == 0x2000) call(chip, opcode);
    else if ((opcode & 0xF000) == 0x3000) skip_if_eq(chip, opcode);
    else if ((opcode & 0xF000) == 0x4000) skip_if_neq(chip, opcode);
    else if ((opcode & 0xF000) == 0x5000) skip_if_reg_eq(chip, opcode);
    else if ((opcode & 0xF000) == 0x6000) store_imd(chip, opcode);
    else if ((opcode & 0xF000) == 0x7000) add_imd(chip, opcode);
    else if ((opcode & 0xF000) == 0x8000) arithmetic(chip, opcode);
    else if ((opcode & 0xF000) == 0x9000) skip_if_reg_neq(chip, opcode);
    else if ((opcode & 0xF000) == 0xA000) index_store(chip, opcode);
    else if ((opcode & 0xF000) == 0xB000) relative_jump(chip, opcode);
    else if ((opcode & 0xF000) == 0xC000) rand_and(chip, opcode);
    else if ((opcode & 0xF000) == 0xD000) draw(chip, opcode);
    else if ((opcode & 0xF000) == 0xE000) skip_if_key(chip, opcode);
    else if ((opcode & 0xF000) == 0xF000) ftype(chip, opcode);
}

void load_font(Chip8* chip) {
    for (uint16_t i = 0; i < FONTSETSIZE; ++i) {
        chip->memory[FONTSETSTART + i] = fontset[i];
    }
}

void init_chip(Chip8* chip) {
    memset(chip->memory, 0, MEMSIZE);
    memset(chip->display, 0, DISPSIZE * 4);
    memset(chip->stack, 0, 16);
    memset(chip->registers, 0, 16);
    memset(chip->keypad, 0, 16);
    chip->pc = PROGSTART;
    chip->index = 0x00;
    chip->sp = 0x00;
    chip->delay_timer = 0x00;
    chip->sound_timer = 0x00;
    chip->draw_f = 0x00;
    load_font(chip);
    srand(0);
}

Chip8* create_chip() {
    Chip8* chip = malloc(sizeof(Chip8));
    init_chip(chip);
    return chip;
}

void delete_chip(Chip8* chip) {
    free(chip);
}

void load_program(Chip8* chip, FILE* program) {
    fseek(program, 0, SEEK_END);
    uint16_t filesize = ftell(program);
    fseek(program, 0, SEEK_SET);

    fread(chip->memory + PROGSTART, filesize, 1, program);
}

int8_t cycle(Chip8* chip) {
    if (chip->pc >= MEMSIZE) return ERR_PC_OUT_OF_RANGE;
    decode_and_exec(chip, fetch(chip));
    if (chip->delay_timer > 0) chip->delay_timer--;
    if (chip->sound_timer > 0) chip->sound_timer--;
    return 0;
}
