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
 * helpfunction in order to write a char
 */
void display_write_char_help(uint8_t number);

/**
 * Writes a single char on the display
 * @param *character
 */
void display_write_char(uint8_t *character);

void display_write_blank_text(uint8_t *text);

void display_write_text(char *text);


#endif //__DISPLAY_H__
