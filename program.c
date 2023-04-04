#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <SDL2/SDL.h>

#include "chip8.h"
#include "display.h"

void milli_delay(uint16_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Not enought arguments!\nQuiting\n");
        return 0;
    }
    Chip8* chip = malloc(sizeof(Chip8));
    init_chip(chip);

    Display* display = create_display(640, 1280, 32, 64);
    init_display(display);

    FILE* program = fopen(argv[1], "rb");
    load_program(chip, program);

    while (1) {
        int8_t key_status = manage_key_press(display, chip->keypad);
        if (key_status == 1 || key_status < 0) break;
        if (chip->pc >= MEMSIZE) break;
        decode_and_exec(chip, fetch(chip));
        update_display(display, chip->display);
        milli_delay(50);
    }

    delete_display(display);
    free(chip);
    fclose(program);
    return 0;
}
