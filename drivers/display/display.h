#include "../../shared/common.h"

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

// Backbuffer
#define DISPLAY_BACKBUFFER_COLUMNS 128
#define DISPLAY_BACKBUFFER_LINES 8

// Initialize display
void display_init(void);
// Set pixel at x, y position, value = 1 = black, value = 0 = black
void display_set_pixel (uint8_t x, uint8_t y, bool value);
// Render whole backbuffer to the screen
void display_flip (void);
// Clear display
void display_clear(void);
#endif
