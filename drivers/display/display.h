/**
 * header for the display driver
 * @file display.h
 * @addtogroup display Display Driver
 * @author Thomas Parsch
*/
/**@{*/

#include <common.h>

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/// Backbuffer width
#define DISPLAY_WIDTH 128
/// Backbuffer height
#define DISPLAY_HEIGHT 64


/** \brief initialize the display
 * calls all required prerequisites for drawing on the displays
 */
void display_init(void);

/** Sets pixel value of coordinate
* @param x coordinate of pixel to set
* @param y coordinate of pixel to set
* @param value set pixel 1 = black, 0 = white, if inverted is not set
*/
void display_set_pixel (uint8_t x, uint8_t y, bool value);

/** Render whole backbuffer to the screen */
void display_flip (void);

/** clears the complete display */
void display_clear(void);

/** \brief set the inverted mode
* inverted mode inverts the color of all pixel drawn after mode is aktivated
* @param value true set inverted mode on, false off
*/
void display_set_inverted(bool value);

/** gets the value of inverted mode status
* @return true if inverted mode is on, false if off
*/
bool display_get_inverted(void);

#endif
/**@}*/
