#include "../../shared/common.h"

#ifndef __DISPLAY_DRAW_H__
#define __DISPLAY_DRAW_H__

void display_draw_char (uint8_t x, uint8_t y, uint8_t font_size, char* asciiIndex);
void display_draw_line (uint8_t xstart ,uint8_t ystart ,uint8_t xend ,uint8_t yend);

#endif
