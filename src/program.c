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
    FILE* program = fopen(argv[1], "rb");

    Display* display = create_display(640, 1280, 32, 64);
    Chip8* chip = create_chip();

    load_program(chip, program);

    while (1) {
        if (manage_key_press(display, chip->keypad) == QUIT) break;
        if (cycle(chip) == ERR_PC_OUT_OF_RANGE) break;
        // debug_print(chip);
        if (chip->draw_f) {
            update_display(display, chip->display);
            chip->draw_f = 0x00;
        }
        milli_delay(5);
    }

    delete_display(display);
    delete_chip(chip);
    fclose(program);

    return 0;
}
