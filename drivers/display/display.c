#include "display.h"
#include <avr/io.h>
#include <util/delay.h>

// LINES
#define DISPLAY_PAGE_NUMBER 7  	//Starts with Page 0, here: 8 pages overall
#define DISPLAY_PAGE_INIT 0

// COLUMNS
// FIXME: Determine real col number
#define DISPLAY_COL_NUMBER 127	//Starts with column 0, here: 132 columns overall
#define DISPLAY_COL_HEIGHT 8
#define DISPLAY_COL_INIT 0 		//normal: 0, reverse: 4
#define DISPLAY_COL_OFFSET 1

// DISPLAY PORTS
#define DISPLAY_CS  PE0	// chip select
#define DISPLAY_RST PC7	// reset
#define DISPLAY_RS  PB3	//Register Select input
#define DISPLAY_RD  PB2	//read
#define DISPLAY_WR  PE1	//write

// Backbuffer
// FIXME: Make consts
#define DISPLAY_BACKBUFFER_COLUMNS 128
#define DISPLAY_BACKBUFFER_LINES 8

enum {
  DISPLAY_CMD,
  DISPLAY_DATA 
}

static uint8_t backbuffer[DISPLAY_BACKBUFFER_LINES][DISPLAY_BACKBUFFER_COLUMNS];

void 
display_init(void)
{
	//According to the init routine in the manual

	//User system setup by external pins
	DDRA = 0xFF;
	set_bit(DDRB, DDB3);
	set_bit(DDRC, DDC7);
	set_bit(DDRE, DDE0);
	set_bit(DDRE, DDE1);

	set_bit(PORTE, DISPLAY_CS);

	//RST Low
	clear_bit(PORTC, DISPLAY_RST);

	//System setup by external pins
	set_bit(PORTB, DISPLAY_RS);
	set_bit(PORTE, DISPLAY_WR);
	set_bit(PORTB, DISPLAY_RD);

	//Waiting for stabilizing power
	_delay_ms(100);

	//RST high
	set_bit(PORTC, DISPLAY_RST);

	//ADC SELECT
	//normal: 0xA0, reverse: 0xA1
	display_send(0xA0);

	//SHL Select
	//normal: 0xC0, reverse: 0xC8
	display_send(0xC0);

	//LCD Bias Select
	display_send(0xA2);

	//Regulator resistor select
	display_send(0x25);

	//------------

	//Set reference voltage mode
	display_send(0x81);
	//Set reference voltage register
	display_send(0x30);

	//------------

	//PowerControl
	display_send(0x2F);

	//DATA DISPLAY PART

	//Initial Display line to 0
	display_send(0x40);
	// SET PAGE ADDRESS 0
	display_send(0xB0);
	//SET COLUMN ADDRESS MSB 0
	display_send(0x10);
	//SET COLUMN ADRESS LSB 0
	display_send(0x00,);

	//REVERSE DISPLAY OFF
	display_send(0xA6, );
	// Display ON
	display_send(0xAF, );

	//Clean Display
	display_clean();
}

void
display_set_pixel(uint8_t x, uint8_t y, bool value)
{
	uint8_t page = y / 8;
	uint8_t col = x;
	uint8_t bit_index = y % 8;
	
	if(value)
		// Black pixel
		backbuffer[page][col] |= 1<<bit_index;
	else
		// White pixel
		backbuffer[page][col] &= ~(1<<bit_index);
}

inline static void 
display_send (uint8_t value, bool data)
{	
	if(data){
		set_bit(PORTB, DISPLAY_RS);		
	}else{
		clear_bit(PORTB, DISPLAY_RS);
	}
	// FIXME: Check interface
	clear_bit(PORTE, DISPLAY_CS);
	clear_bit(PORTE, DISPLAY_WR);
	set_bit(PORTB, DISPLAY_RD);
	PORTA = value;
	set_bit(PORTE, DISPLAY_WR);
	set_bit(PORTE, DISPLAY_CS);
}

inline static void 
display_set_col(uint8_t col)
{
	display_send(0x10 + (col >> 4));		// MSB
	display_send(0x00 + (col & 0x0F));	// LSB
}

inline static void 
display_set_page(uint8_t page)
{
	//Top down instead of bottom up
	uint8_t inverted_page = DISPLAY_PAGE_NUMBER - page;
	display_send(0xB0 + inverted_page);
}

void
display_flip(void)
{
	for(uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++){
		display_set_page(page);
		display_set_col(0);
		for(uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++){
			display_cmd(backbuffer[page][col], 1);
		}
	}
}
