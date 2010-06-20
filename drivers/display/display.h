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
	DISPLAY_TITLE_RIGHT = 0x01,
	DISPLAY_TITLE_BOTH = 0x81
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
	DIPSLAY_TEXT_BOTTOM = 0x01,
	DISPLAY_TEXT_BOTH = 0x81

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
 * Writes a single char on the display
 * @param *character
 */
void display_write_char(uint8_t *character, uint8_t inverse);

void display_write_title(char *text, uint8_t status);

void display_write_text(char *text, uint8_t status);

#endif //__DISPLAY_H__
