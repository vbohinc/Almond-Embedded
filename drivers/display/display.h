/**
 * @file display.h
 * @author parsch
 * @date 2010/06/2
 *
 * The display API
 */

#include <avr/io.h>
#include <util/delay.h>
#include "../../shared/common.h"
#include "display_widget.h"

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/*
 *Status Titlebar
 *|Left|X|X|X|X|X|X|Right|
 */
enum
{
	DISPLAY_TITLE_NONE = 0x00,
	DISPLAY_TITLE_LEFT = 0x80,
	DISPLAY_TITLE_RIGHT = 0x01
};

/*
 *
 *Status TextArea
 *|Top|X|X|X|X|X|X|Bottom|
 */
enum
{
	DISPLAY_TEXT_NONE = 0x00,
	DISPLAY_TEXT_TOP = 0x80,
	DIPSLAY_TEXT_BOTTOM = 0x01

};

/**
 * Special Chars
 */
enum
{
	DISPLAY_CHAR_ARROW_LEFT = 54,
	DISPLAY_CHAR_ARROW_RIGHT = 55,
	DISPLAY_CHAR_ARROW_TOP_SMALL = 56,
	DISPLAY_CHAR_ARROW_BOTTOM_SMALL = 57,
	DISPLAY_CHAR_ARROW_TOP_AND_BOTTOM_SMALL = 58
};

/**
 *
 * Initialization of the display
 *
 */
void display_init(void);

/**
 * Cleans the display - nothing is to see anymore
 */
void display_clean(void);

/**
 * Deletes a certain char (does not effect the boarder pixels)
 * @param row line of the char (starts with 0)
 * @param symbol number of the symbol in the line (starts with 0)
 *  */
void display_clean_char(uint8_t line, uint8_t symbol, uint8_t inverse_modus);

/**
 * Deletes a certain row (does not effect the boarder pixels)
 * @param line line to delete (starts with 0)
 */
void display_clean_line(uint8_t line, uint8_t inverse_modus);

/**
 * Writes a single char on the display
 * @param *character
 * @param inverse 0 normal, 1 (not 0) inverse
 */
void display_write_char(uint8_t character, uint8_t inverse_modus);

/**
 * Write the title (inverted)
 * @param *text Array of chars to be written
 * @param status Controls the left and the right arrow
 */
void display_write_title(const char *text, uint8_t status);

/**
 * Writes a text on the display
 * @param *character arry of chars to be written
 * @param status of the bottom/top arrows
 */
void display_write_text(const char *text, uint8_t status);


#endif //__DISPLAY_H__
