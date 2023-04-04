#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "display.h"

Display* create_display(uint16_t win_h, uint16_t win_w, uint16_t img_h, uint16_t img_w) {
    Display* display = malloc(sizeof(Display));

    display->window = NULL;
    display->renderer = NULL;
    display->texture = NULL;

    display->window_width = win_w;
    display->window_height = win_h;
    display->image_width = img_w;
    display->image_height = img_h;

    return display;
}

int8_t init_display(Display* display) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initiliaze. Error: %s\n", SDL_GetError());
        return -1;
    } 

    display->window = SDL_CreateWindow("Test SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                        display->window_width, display->window_height, SDL_WINDOW_SHOWN);
    if (display->window < 0) {
        printf("Window could not be created. Error: %s\n", SDL_GetError());
        return -1;
    }

    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer < 0) {
        printf("Renderer could not be created. Error: %s\n", SDL_GetError());
        return -1;
    }

    display->texture = SDL_CreateTexture(display->renderer, 
                                         SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 
                                         display->image_width, display->image_height);
    if (display->texture < 0) {
        printf("Texture could not be created. Error: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

void set_keypad_chip8(uint8_t* keypad, uint16_t scancode, uint8_t val) {
    if (scancode == SDL_SCANCODE_1) keypad[0] = val;
    else if (scancode == SDL_SCANCODE_2) keypad[1] = val;
    else if (scancode == SDL_SCANCODE_3) keypad[2] = val;
    else if (scancode == SDL_SCANCODE_4) keypad[3] = val;
    else if (scancode == SDL_SCANCODE_Q) keypad[4] = val;
    else if (scancode == SDL_SCANCODE_W) keypad[5] = val;
    else if (scancode == SDL_SCANCODE_E) keypad[6] = val;
    else if (scancode == SDL_SCANCODE_R) keypad[7] = val;
    else if (scancode == SDL_SCANCODE_A) keypad[8] = val;
    else if (scancode == SDL_SCANCODE_S) keypad[9] = val;
    else if (scancode == SDL_SCANCODE_D) keypad[10] = val;
    else if (scancode == SDL_SCANCODE_F) keypad[11] = val;
    else if (scancode == SDL_SCANCODE_Z) keypad[12] = val;
    else if (scancode == SDL_SCANCODE_X) keypad[13] = val;
    else if (scancode == SDL_SCANCODE_C) keypad[14] = val;
    else if (scancode == SDL_SCANCODE_V) keypad[15] = val;
}

int8_t manage_key_press(Display* display, uint8_t* keypad) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 1;
        if (event.type == SDL_KEYDOWN) set_keypad_chip8(keypad, event.key.keysym.scancode, 1);
        else if (event.type == SDL_KEYUP) set_keypad_chip8(keypad, event.key.keysym.scancode, 0);
    }
    return 0;
}

void update_display(Display* display, const uint32_t* new_pixels) {
    SDL_UpdateTexture(display->texture, NULL, new_pixels, sizeof(uint32_t) * display->image_width);
    SDL_RenderClear(display->renderer);
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}

void delete_display(Display* display) {
    SDL_DestroyTexture(display->texture);
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
    free(display);
}
