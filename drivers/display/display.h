/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
     _____                      ___           ___                       ___                 
    /  /::\       ___          /  /\         /  /\                     /  /\          ___   
   /  /:/\:\     /  /\        /  /:/_       /  /::\                   /  /::\        /__/|  
  /  /:/  \:\   /  /:/       /  /:/ /\     /  /:/\:\  ___     ___    /  /:/\:\      |  |:|  
 /__/:/ \__\:| /__/::\      /  /:/ /::\   /  /:/~/:/ /__/\   /  /\  /  /:/~/::\     |  |:|  
 \  \:\ /  /:/ \__\/\:\__  /__/:/ /:/\:\ /__/:/ /:/  \  \:\ /  /:/ /__/:/ /:/\:\  __|__|:|  
  \  \:\  /:/     \  \:\/\ \  \:\/:/~/:/ \  \:\/:/    \  \:\  /:/  \  \:\/:/__\/ /__/::::\  
   \  \:\/:/       \__\::/  \  \::/ /:/   \  \::/      \  \:\/:/    \  \::/         ~\~~\:\ 
    \  \::/        /__/:/    \__\/ /:/     \  \:\       \  \::/      \  \:\           \  \:\
     \__\/         \__\/       /__/:/       \  \:\       \__\/        \  \:\           \__\/
                               \__\/         \__\/                     \__\/                
*/

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

//0 means no visible chars
#define DISPLAY_CHAR_MAX ((DISPLAY_COL_NUMBER_VISIBLE + 1)/ DISPLAY_CHAR_WIDTH)


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
 *|Top|X|X|DEBUG|X|X|X|Bottom|
 */
enum
{
	DISPLAY_TEXT_NONE = 0x00,
	DISPLAY_TEXT_TOP = 0x80,
	DISPLAY_TEXT_BOTTOM = 0x01,
	DISPLAY_TEXT_DEBUG = 0x10
};

/**
 * Special Chars
 */
enum
{
	DISPLAY_CHAR_ARROW_LEFT = 47,
	DISPLAY_CHAR_ARROW_RIGHT = 48,
	DISPLAY_CHAR_ARROW_TOP_SMALL = 49,
	DISPLAY_CHAR_ARROW_BOTTOM_SMALL = 50,
	DISPLAY_CHAR_ARROW_TOP_AND_BOTTOM_SMALL = 51,
	DISPLAY_CHAR_SPINNER_DOT = 52,
	DISPLAY_CHAR_SPINNER_DOT_FULL = 53
};

/**
 * MODES
 * Helps to control the write_text
 */
enum
{
	DISPLAY_MODE_NORMAL = 0,
	DISPLAY_MODE_INVERSE = 1
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
 * Writes the status arrows in the last char field
 * @param status of the bottom/top arrows
 */
void display_write_text_status(uint8_t status);

/**
 * Writes a text on the display
 * @param *character array of chars to be written
 * @param status of the bottom/top arrows
 */
void display_write_text(const char *text, uint8_t status);

/**
 * Widget to show a start up screen
 */
void display_spinner(void);

#endif //__DISPLAY_H__
