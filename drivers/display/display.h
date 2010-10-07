#include "../../shared/common.h"

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

void display_init(void);
// Set pixel at x, y position, value = 1 = black, value = 0 = black
void display_set_pixel (uint8_t x, uint8_t y, bool value);
// Render whole backbuffer to the screen
void display_flip (void);

#endif
