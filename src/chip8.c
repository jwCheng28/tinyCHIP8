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

void jump(Chip8* chip, uint16_t opcode) {
    chip->pc = opcode & 0x0FFF;
}

void call(Chip8* chip, uint16_t opcode) {
    chip->stack[chip->sp] = chip->pc;
    chip->pc = opcode & 0x0FFF;
    chip->sp++;
}

void skip_if_eq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    if (rx == (opcode & 0x00FF)) chip->pc++;
}

void skip_if_neq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    if (rx != (opcode & 0x00FF)) chip->pc++;
}

void skip_if_reg_eq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    uint8_t ry = chip->registers[(opcode & 0x00F0) >> 4];
    if (rx == ry) chip->pc++;
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
        chip->registers[xid] = sum & 0x00FF;
        chip->registers[0xF] = !!(sum & 0x0F00);
    } else if ((opcode & 0x000F) == 0x0005) {
        chip->registers[xid] -= ry;
        chip->registers[0xF] = !!(rx > ry);
    } else if ((opcode & 0x000F) == 0x0006) {
        chip->registers[0xF] = rx & 0x01;
        chip->registers[xid] >>= 1;
    } else if ((opcode & 0x000F) == 0x0007) {
        chip->registers[xid] = rx - ry;
        chip->registers[0xF] = !!(ry > rx);
    } else if ((opcode & 0x000F) == 0x000E) {
        chip->registers[0xF] = rx & 0x80;
        chip->registers[xid] <<= 1;
    } else {
        printf("Program Error\nQuiting...\n");
        exit(0);
    }
}

void skip_if_reg_neq(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8];
    uint8_t ry = chip->registers[(opcode & 0x00F0) >> 4];
    if (rx != ry) chip->pc++;
}

void index_store(Chip8* chip, uint16_t opcode) {
    chip->index = opcode & 0x0FFF;
}

void relative_jump(Chip8* chip, uint16_t opcode) {
    chip->pc = (opcode & 0x0FFF) + chip->registers[0];
}

void draw(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8] % DISPWIDTH;
    uint8_t ry = chip->registers[(opcode & 0x00F0) >> 4] % DISPHEIGHT;
    uint8_t nsize = opcode & 0x000F;
    uint16_t disp_index;
    uint8_t byte, row, col;
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
}

void skip_if_key(Chip8* chip, uint16_t opcode) {
    uint8_t rx = chip->registers[(opcode & 0x0F00) >> 8] % DISPWIDTH;
    if ( ((opcode & 0x00FF) == 0x9E && rx) || ((opcode & 0x00FF) == 0xA1 && !rx) ) chip->pc += 2; 
}

uint16_t fetch(Chip8* chip) {
    return (chip->memory[chip->pc++] << 8) + chip->memory[chip->pc++];
}

void decode_and_exec(Chip8* chip, uint16_t opcode) {
    if ((opcode & 0xFF0F) == 0x0000) memset(chip->display, 0, DISPSIZE * 4);
    if ((opcode & 0xFF00) == 0x0000) {}
    if ((opcode & 0xF000) == 0x0000) {}
    if ((opcode & 0xF000) == 0x1000) jump(chip, opcode);
    if ((opcode & 0xF000) == 0x2000) call(chip, opcode);
    if ((opcode & 0xF000) == 0x3000) skip_if_eq(chip, opcode);
    if ((opcode & 0xF000) == 0x4000) skip_if_neq(chip, opcode);
    if ((opcode & 0xF000) == 0x5000) skip_if_reg_eq(chip, opcode);
    if ((opcode & 0xF000) == 0x6000) store_imd(chip, opcode);
    if ((opcode & 0xF000) == 0x7000) add_imd(chip, opcode);
    if ((opcode & 0xF000) == 0x8000) arithmetic(chip, opcode);
    if ((opcode & 0xF000) == 0x9000) skip_if_reg_neq(chip, opcode);
    if ((opcode & 0xF000) == 0xA000) index_store(chip, opcode);
    if ((opcode & 0xF000) == 0xB000) relative_jump(chip, opcode);
    if ((opcode & 0xF000) == 0xC000) {}
    if ((opcode & 0xF000) == 0xD000) draw(chip, opcode);
    if ((opcode & 0xF000) == 0xE000) skip_if_key(chip, opcode);
    if ((opcode & 0xF000) == 0xF000) {}
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
    load_font(chip);
}

void load_program(Chip8* chip, FILE* program) {
    fseek(program, 0, SEEK_END);
    uint16_t filesize = ftell(program);
    fseek(program, 0, SEEK_SET);

    fread(chip->memory + PROGSTART, filesize, 1, program);
}
