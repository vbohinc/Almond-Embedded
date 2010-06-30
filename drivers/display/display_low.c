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

#include "display_low.h"

void display_command(uint8_t command)
{
	clear_bit(PORTB, DISPLAY_RS);
	clear_bit(PORTE, DISPLAY_CS);
	clear_bit(PORTE, DISPLAY_WR);
	set_bit(PORTB, DISPLAY_RD);
	PORTA = command;
	set_bit(PORTE, DISPLAY_WR);
	set_bit(PORTE, DISPLAY_CS);
}

void display_write(uint8_t value, uint8_t inverse)
{
	set_bit(PORTB, DISPLAY_RS);
	clear_bit(PORTE, DISPLAY_CS);
	clear_bit(PORTE, DISPLAY_WR);
	set_bit(PORTB, DISPLAY_RD);
	if (inverse)
	{
		value = ~value;
	}
	PORTA = value;
	set_bit(PORTE, DISPLAY_WR);
	set_bit(PORTE, DISPLAY_CS);
}

void display_set_col(uint8_t col)
{
	// MSB
	display_command(0x10 + (col >> 4));
	// LSB
	display_command(0x00 + (col & 0x0F));
}

void display_set_page(uint8_t page)
{
	//Top down instead of bottom up
	uint8_t inverted_page = DISPLAY_PAGE_NUMBER - page;
	display_command(0xB0 + inverted_page);
}
