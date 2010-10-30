#include <common.h>

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

// Backbuffer
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

void display_init(void);
// Set pixel at x, y position, value = 1 = black, value = 0 = black
void display_set_pixel (uint8_t x, uint8_t y, bool value);
// Render whole backbuffer to the screen
void display_flip (void);
void display_clear(void);

void display_set_inverted(bool value);
bool display_get_inverted(void);

#endif
