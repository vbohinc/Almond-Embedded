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

#include <avr/io.h>
#include "../../shared/common.h"

#ifndef __DISPLAY_LOW_H__
#define __DISPLAY_LOW_H__

//PAGE
#define DISPLAY_PAGE_NUMBER 7  //Starts with Page 0, here: 8 pages overall
#define DISPLAY_PAGE_INIT 0

//COLUMN
#define DISPLAY_COL_NUMBER 131  //Starts with column 0, here: 132 colums overall
#define DISPLAY_COL_NUMBER_VISIBLE 127
#define DISPLAY_COL_HEIGHT 8
#define DISPLAY_COL_INIT 0 //normal: 0, reverse: 4
#define DISPLAY_COL_OFFSET 1


//CHARACTER
#define DISPLAY_CHAR_WIDTH 6



// DISPLAY PORTS
// chip select
#define DISPLAY_CS  PE0

// reset
#define DISPLAY_RST PC7

//Register Select input
#define DISPLAY_RS  PB3

//read
#define DISPLAY_RD  PB2

//write
#define DISPLAY_WR  PE1

/**
 * Sends a command to the display
 * @param command command to be sent
 */
void display_command(uint8_t command);

/**
 * Writes a value to the lcd-display
 * @param value value to be written
 */
void display_write(uint8_t value, uint8_t inverse_modus);

/**
 *  Sets Column
 *  @param col column to be set
 */
void display_set_col(uint8_t col);

/**
 * Sets Page
 * @param page page to be set
 */
void display_set_page(uint8_t page);

#endif //__DISPLAY_LOW_H__
