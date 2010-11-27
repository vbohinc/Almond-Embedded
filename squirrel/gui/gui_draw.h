/**
 * Highlevel Display Driver
 * @file gui_draw.h
 * @author Matthias Schwab
 */
#include <common.h>

#ifndef __DISPLAY_DRAW_H__
#define __DISPLAY_DRAW_H__

#define DISPLAY_FONT0_HEIGHT 8
#define DISPLAY_FONT0_WIDTH 6

/**
* Draws a character at a given (top,left) position
* @param x position coordiante
* @param y position coordinate
* @param font_size of the character
* @param asciiIndex character value in ascii encoding
*/
void display_draw_char (uint8_t x, uint8_t y, uint8_t font_size, char asciiIndex);

/**
* Draws a whole string at a given (top,left) position
* @param x position coordiante
* @param y position coordinate
* @param font_size of the string
* @param cha_array containing the string to draw
*/
void display_draw_string (uint8_t x, uint8_t y, uint8_t font_size, const char* char_array);

/**
* Draws a whole string with a delay between each character (animation like)
* @param x position coordiante
* @param y position coordinate
* @param font_size of the string
* @param cha_array containing the string to draw
* @param delay time between each drawing of a character
*/
void display_draw_string_delayed(uint8_t x, uint8_t y, uint8_t font_size, const char* char_array, uint16_t delay);

/**
* Draws a (filled or not filled) rectangle (x1,y1 = top/left-corner, x2,y2 = bottom/right-corner)
* @param x1 top left corner x position coordiante
* @param y1 top left corner y position coordinate
* @param x2 bottom right corner x position coordiante
* @param y2 bottom right corner y position coordinate
* @param filled should be true to draw a filled rectangle or false for borders only
*/
void display_draw_rect (uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool filled);

/**
* Draws a line from xstart, ystart to xend, yend
* @param xstart line start x position coordinate
* @param ystart line start y position coordinate
* @param xend line end x position coordinate
* @param yend line end y position coordinate
*/
void display_draw_line (uint8_t xstart, uint8_t ystart, uint8_t xend, uint8_t yend);

/**
* Prints to the (simulated) console
* @param char_array string to print
*/
void display_print(const char* char_array);

/**
* Draws an image array to screen
* @param topx top x position coordinate to draw image to
* @param topy top y position coordinate to draw image to
* @parma image_array uint8 array containing pixel information of image
*/
void display_draw_image(int16_t topx, int16_t topy, const uint8_t* image_array);

/**
* Draw image with frames
* @param topx top x position coordinate to draw image to
* @param topy top y position coordinate to draw image to
* @param franes_array 2 dimensional array with all the frames images
* @param rounds to draw all frames of animation
* @param sleep time between each frame to wait
*/
void display_draw_animated_image(int8_t x, int8_t y, const uint8_t** frames_array, uint8_t rounds, uint16_t sleep);

/**
* Draws a frame (round_counter) of an animation array if round_counter > number of frames draws frame 0. return value is next frame
* @param topx top x position coordinate to draw image to
* @param topy top y position coordinate to draw image to
* @param franes_array 2 dimensional array with all the frames images
* @param round_counter gives the frame to be drawn, if bigger than actual fames count it is resetted to 0
* @return the actual frame number that was drawn
*/
uint8_t display_draw_animation_frame(int8_t x, int8_t y, const uint8_t** frames_array, uint8_t round_counter);

#endif
