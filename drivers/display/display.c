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
#define DISPLAY_CS  6 // chip select
#define DISPLAY_RST 7 // reset
#define DISPLAY_RS  5 //Register Select input
#define DISPLAY_RD  3 //read
#define DISPLAY_WR  4 //write

// Backbuffer
// FIXME: Make consts
#define DISPLAY_BACKBUFFER_COLUMNS 128
#define DISPLAY_BACKBUFFER_LINES 8

// Display command type
enum {
  DISPLAY_CMD,
  DISPLAY_DATA 
};
void display_test(void);

inline static void display_send (uint8_t value, bool data);

static uint8_t backbuffer[DISPLAY_BACKBUFFER_LINES][DISPLAY_BACKBUFFER_COLUMNS];

void 
display_init(void)
{
	//User system setup by external pins
	PORTF.DIR = 0xFF;
	set_bit(PORTH.DIR, DISPLAY_RS);
	set_bit(PORTH.DIR, DISPLAY_RST);
	set_bit(PORTH.DIR, DISPLAY_CS);
	set_bit(PORTH.DIR, DISPLAY_WR);

	set_bit(PORTH.OUT, DISPLAY_CS);


	clear_bit(PORTH.OUT, DISPLAY_RST);


	set_bit(PORTH.OUT, DISPLAY_RS);
	set_bit(PORTH.OUT, DISPLAY_WR);
	set_bit(PORTH.OUT, DISPLAY_RD);

	_delay_ms(100);				//Waiting for stabilizing power
	set_bit(PORTH.OUT, DISPLAY_RST);

	display_send(0xA0, DISPLAY_DATA);	//ADC SELECT
	display_send(0xC0, DISPLAY_DATA);	//SHL Select
	display_send(0xA2, DISPLAY_DATA);	//LCD Bias Select
	display_send(0x25, DISPLAY_DATA);	//Regulator resistor select

	display_send(0x81, DISPLAY_DATA);	//Set reference voltage mode
	display_send(0x30, DISPLAY_DATA);	//Set reference voltage register

	display_send(0x2F, DISPLAY_DATA);	//PowerControl

	display_send(0x40, DISPLAY_DATA);	//Initial Display line to 0
	display_send(0xB0, DISPLAY_DATA);	// SET PAGE ADDRESS 0
	display_send(0x10, DISPLAY_DATA);	//SET COLUMN ADDRESS MSB 0
	display_send(0x00, DISPLAY_DATA);	//SET COLUMN ADRESS LSB 0
	display_send(0xA6, DISPLAY_DATA);	//REVERSE DISPLAY OFF
	display_send(0xAF, DISPLAY_DATA);	// Display ON
}

void
display_set_pixel(uint8_t x, uint8_t y, bool value)
{
	uint8_t page = y / 8;
	uint8_t col = x;
	uint8_t bit_index = (y % 8);
	
	if(x >= DISPLAY_BACKBUFFER_COLUMNS) x = DISPLAY_BACKBUFFER_COLUMNS - 1;
	if(y >= 64) x = 63;
	
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
		set_bit(PORTH.OUT, DISPLAY_RS);		
	}else{
		clear_bit(PORTH.OUT, DISPLAY_RS);
	}
	// FIXME: Check interface
	clear_bit(PORTH.OUT, DISPLAY_CS);
	clear_bit(PORTH.OUT, DISPLAY_WR);
	set_bit(PORTH.OUT, DISPLAY_RD);
	value = (value & 0x55) << 1 | ((value >> 1) & 0x55); 
	value = (value & 0x33) << 2 | ((value >> 2) & 0x33); 
	PORTF.OUT = (value & 0x0F) << 4 | ((value >> 4) & 0x0F); 
	//PORTF.OUT = ((value&0x01)<<7)|((value&0x02)<<5)|((value&0x04)<<3)|
	//		((value&0x08)<<1)|((value&0x10)>>1)|((value&0x20)>>3)|
	//		((value&0x40)>>5)|((value&0x80)>>7);
	set_bit(PORTH.OUT, DISPLAY_WR);
	set_bit(PORTH.OUT, DISPLAY_CS);
}

inline static void 
display_set_col(uint8_t col)
{
	display_send(0x10 + (col >> 4), DISPLAY_CMD);		// MSB
	display_send(0x00 + (col & 0x0F), DISPLAY_CMD);	// LSB
}

inline static void 
display_set_page(uint8_t page)
{
	//Top down instead of bottom up
	uint8_t inverted_page = DISPLAY_PAGE_NUMBER - page;
	display_send(0xB0 + inverted_page, DISPLAY_CMD);
}

void
display_flip(void)
{
	for(uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++){
		display_set_page(page);
		display_set_col(0);
		for(uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++){
			display_send (backbuffer[page][col], DISPLAY_DATA);
		}
	}
}

void
display_clear(void)
{
	for(uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++){
		display_set_page(page);
		display_set_col(0);
		for(uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++){
			display_send (0, DISPLAY_DATA);
		}
	}
}
