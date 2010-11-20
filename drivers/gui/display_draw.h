/**
 * display_draw.h - Highlevel Display Driver
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Matthias Schwab
 */
#include "../../shared/common.h"

#ifndef __DISPLAY_DRAW_H__
#define __DISPLAY_DRAW_H__

#define DISPLAY_FONT0_HEIGHT 8
#define DISPLAY_FONT0_WIDTH 6

// Draw a character at a given (top,left) position
void display_draw_char (uint8_t x, uint8_t y, uint8_t font_size, char asciiIndex);
// Draw a whole string at a given (top,left) position
void display_draw_string (uint8_t x, uint8_t y, uint8_t font_size, const char* char_array);
// Draw a whole string with a delay between each character
void display_draw_string_delayed(uint8_t x, uint8_t y, uint8_t font_size, const char* char_array, uint16_t delay);
// Draw a (not filled) rectangle (x1,y1 = top/left-corner, x2,y2 = bottom/right-corner)
void display_draw_rect (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool filled);
// Draw a line from xstart, ystart to xend, yend
void display_draw_line (uint8_t xstart, uint8_t ystart, uint8_t xend, uint8_t yend);
// Print to the console
void display_print(const char* char_array);
// Draw image array to screen
void display_draw_image(int16_t topx, int16_t topy, const uint8_t* image_array);
// Draw image with frames
void display_draw_animated_image(int8_t x, int8_t y, const uint8_t** frames_array, uint8_t rounds, uint16_t sleep);
// Draws a frame (round_counter) of an animation array if round_counter > number of frames draws frame 0. return value is next frame
uint8_t display_draw_animation_frame(int8_t x, int8_t y, const uint8_t** frames_array, uint8_t round_counter);

#endif
