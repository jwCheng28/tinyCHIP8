#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint16_t window_width;
    uint16_t window_height;
    uint16_t image_width;
    uint16_t image_height;
} Display;

Display* create_display(uint16_t win_h, uint16_t win_w, uint16_t img_h, uint16_t img_w);
int8_t init_display(Display* display);
int8_t manage_key_press(Display* display, uint8_t* keypad);
void update_display(Display* display, const uint32_t* new_pixels);
void delete_display(Display* display);

#endif
