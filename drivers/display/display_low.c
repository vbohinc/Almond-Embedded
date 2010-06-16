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

void display_write(uint8_t value)
{
	set_bit(PORTB, DISPLAY_RS);
	clear_bit(PORTE, DISPLAY_CS);
	clear_bit(PORTE, DISPLAY_WR);
	set_bit(PORTB, DISPLAY_RD);
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
	display_command(0xB0 + page);
}
