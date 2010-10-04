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

#include "display.h"
#include "display_low.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>

// Backbuffer
static uint8_t backbuffer[][]

//Special Char enum in display.h defined
static const uint8_t charray[][6] PROGMEM =
{
//0
		{ 0x0, 0x3e, 0x45, 0x49, 0x51, 0x3e },
		//1
		{ 0x0, 0x0, 0x10, 0x20, 0x7f, 0x0 },
		//2
		{ 0x0, 0x21, 0x43, 0x45, 0x49, 0x31 },
		//3
		{ 0x0, 0x41, 0x49, 0x49, 0x7f, 0x0 },
		//4
		{ 0x0, 0x7c, 0x4, 0x4, 0x1f, 0x4 },
		//5
		{ 0x0, 0x79, 0x49, 0x49, 0x49, 0x4f },
		//6
		{ 0x0, 0x7f, 0x49, 0x49, 0x49, 0x4f },
		//7
		{ 0x0, 0x40, 0x48, 0x48, 0x7f, 0x8 },
		//8
		{ 0x0, 0x7f, 0x49, 0x49, 0x49, 0x7f },
		//9
		{ 0x0, 0x79, 0x49, 0x49, 0x49, 0x7f },
		//10:A
		{ 0x0, 0x7f, 0x48, 0x48, 0x48, 0x7f },
		//11:B
		{ 0x0, 0x7f, 0x49, 0x49, 0x49, 0x3e },
		//12:C
		{ 0x0, 0x7f, 0x41, 0x41, 0x41, 0x41 },
		//13:D
		{ 0x0, 0x7f, 0x41, 0x41, 0x41, 0x3e },
		//14:E
		{ 0x0, 0x7f, 0x49, 0x49, 0x49, 0x41 },
		//15:F
		{ 0x0, 0x7f, 0x48, 0x48, 0x48, 0x40 },
		//16:G
		{ 0x0, 0x7f, 0x41, 0x49, 0x49, 0x4f },
		//17:H
		{ 0x0, 0x7f, 0x8, 0x8, 0x8, 0x7f },
		//18:I
		{ 0x0, 0x0, 0x41, 0x7f, 0x41, 0x0 },
		//19:J
		{ 0x0, 0x42, 0x41, 0x41, 0x7e, 0x0 },
		//20:K
		{ 0x0, 0x7f, 0x8, 0x14, 0x22, 0x41 },
		//L
		{ 0x0, 0x7f, 0x1, 0x1, 0x1, 0x1 },
		//M
		{ 0x0, 0x7f, 0x20, 0x10, 0x20, 0x7f },
		//N
		{ 0x0, 0x7f, 0x10, 0x8, 0x4, 0x7f },
		//O
		{ 0x0, 0x3e, 0x41, 0x41, 0x41, 0x3e },
		//25:P
		{ 0x0, 0x7f, 0x48, 0x48, 0x48, 0x78 },
		//Q
		{ 0x0, 0x7f, 0x41, 0x43, 0x7f, 0x1 },
		//R
		{ 0x0, 0x7f, 0x4c, 0x4c, 0x4a, 0x31 },
		//S
		{ 0x0, 0x79, 0x49, 0x49, 0x49, 0x4f },
		//T
		{ 0x0, 0x40, 0x40, 0x7f, 0x40, 0x40 },
		//30:U
		{ 0x0, 0x7f, 0x1, 0x1, 0x1, 0x7f },
		//V
		{ 0x0, 0x78, 0x6, 0x1, 0x6, 0x78 },
		//W
		{ 0x0, 0x7f, 0x1, 0xf, 0x1, 0x7f },
		//X
		{ 0x0, 0x41, 0x22, 0x1c, 0x22, 0x41 },
		//Y
		{ 0x0, 0x40, 0x20, 0x1f, 0x20, 0x40 },
		//35:Z
		{ 0x0, 0x43, 0x45, 0x49, 0x51, 0x61 },
		//degree
		{ 0x0, 0x0, 0x0, 0x70, 0x50, 0x70 },
		//. dot
		{ 0x0, 0x0, 0x0, 0x3, 0x3, 0x0 },
		//: double dot
		{ 0x0, 0x0, 0x0, 0x33, 0x33, 0x0 },
		//, comma
		{ 0x0, 0x4, 0x7, 0x0, 0x0, 0x0 },
		//40:_ underscore
		{ 0x0, 0x1, 0x1, 0x1, 0x1, 0x1 },
		//- minus
		{ 0x0, 0x8, 0x8, 0x8, 0x8, 0x8 },
		// + plus
		{ 0x0, 0x8, 0x8, 0x3e, 0x8, 0x8 },
		// unknown full
		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
		// blank
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		//45: % percent
		{ 0x0, 0x62, 0x64, 0x8, 0x13, 0x23 },
		// exclamation mark
		{ 0x0, 0x0, 0x7b, 0x7b, 0x0, 0x0 },
		// arrow left
		{ 0x0, 0x0, 0x8, 0x1c, 0x3e, 0x0 },
		// arrow right
		{ 0x0, 0x0, 0x3e, 0x1c, 0x8, 0x0 },
		//arrow top
		{ 0x0, 0x10, 0x30, 0x70, 0x30, 0x10 },
		//arrow down
		{ 0x0, 0x4, 0x6, 0x7, 0x6, 0x4 },
		//50: arrows bottom/top
		{ 0x0, 0x14, 0x36, 0x77, 0x36, 0x14 },
		// spinner dot
		{ 0x0, 0x3c, 0x24, 0x24, 0x3c, 0x0 },
		//53: spinner dot full
		{ 0x0, 0x3c, 0x3c, 0x3c, 0x3c, 0x0 }

};

void display_init(void)
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
	display_command(0xA0);

	//SHL Select
	//normal: 0xC0, reverse: 0xC8
	display_command(0xC0);

	//LCD Bias Select
	display_command(0xA2);

	//Regulator resistor select
	display_command(0x25);

	//------------

	//Set reference voltage mode
	display_command(0x81);
	//Set reference voltage register
	display_command(0x30);

	//------------

	//PowerControl
	display_command(0x2F);

	//DATA DISPLAY PART

	//Initial Display line to 0
	display_command(0x40);
	// SET PAGE ADDRESS 0
	display_command(0xB0);
	//SET COLUMN ADDRESS MSB 0
	display_command(0x10);
	//SET COLUMN ADRESS LSB 0
	display_command(0x00);

	//REVERSE DISPLAY OFF
	display_command(0xA6);
	// Display ON
	display_command(0xAF);

	//Clean Display
	display_clean();
}

void display_clean(void)
{
	uint8_t i;
	for (i = 0; i <= DISPLAY_PAGE_NUMBER; i++)
	{
		display_set_page(i);
		display_set_col(0);
		uint8_t j;
		// only 128 visible
		for (j = 0; j <= DISPLAY_COL_NUMBER; j++)
		{
			display_write(0x00, DISPLAY_MODE_NORMAL);
		}

	}
	display_set_col(DISPLAY_COL_INIT);
	display_set_page(DISPLAY_PAGE_INIT);
}

void display_clean_char(uint8_t line, uint8_t symbol, uint8_t inverse_modus)
{
	uint8_t blank = (uint8_t) ' ';
	if (line <= DISPLAY_PAGE_NUMBER && symbol <= DISPLAY_CHAR_MAX)
	{
		display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET
				+ DISPLAY_CHAR_WIDTH * symbol);
		display_set_page(DISPLAY_PAGE_INIT + line);
		display_write_char(blank, inverse_modus);
	}
}

void display_clean_line(uint8_t line, uint8_t inverse_modus)
{
	uint8_t blank = (uint8_t) ' ';
	uint8_t i;
	if (line <= DISPLAY_PAGE_NUMBER)
	{
		display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);
		display_set_page(DISPLAY_PAGE_INIT + line);
		for (i = 0; i < DISPLAY_CHAR_MAX; i++)
		{
			display_write_char(blank, inverse_modus);
		}

	}

}

/**
 * help function in order to write a char
 */

static void display_util_write_char(uint8_t number, uint8_t inverse_modus)
{
	uint8_t i;
	for (i = 0; i < 6; i++)
	{
		display_write(pgm_read_byte(&charray[number][i]), inverse_modus);
	}
}

void display_write_char(uint8_t character, uint8_t inverse_modus)
{
	if (character != '\0')
	{
		if (character >= 'A' && character <= 'Z')
		{
			display_util_write_char(character - 55, inverse_modus); // A-Z
		}
		else if (character >= 'a' && character <= 'z')
		{
			display_util_write_char(character - 87, inverse_modus); //A-Z
		}
		else if (character >= '0' && character <= '9')
		{
			display_util_write_char(character - 48, inverse_modus); //0-9
		}
		else if (character == ' ')
		{
			display_util_write_char(44, inverse_modus); //blank
		}
		else if (character == '°')
		{
			display_util_write_char(36, inverse_modus); //degree
		}
		else if (character == '.')
		{
			display_util_write_char(37, inverse_modus); //dot
		}
		else if (character == ':')
		{
			display_util_write_char(38, inverse_modus); //double dot
		}
		else if (character == ',')
		{
			display_util_write_char(39, inverse_modus); //comma
		}
		else if (character == '-')
		{
			display_util_write_char(41, inverse_modus); //minus
		}
		else if (character == '+')
		{
			display_util_write_char(42, inverse_modus); //plus
		}
		else if (character == '%')
		{
			display_util_write_char(45, inverse_modus); //Percent
		}
		else if (character == '!')
		{
			display_util_write_char(46, inverse_modus); //exclamationmark
		}
		else if (character == '_')
		{
			display_util_write_char(40, inverse_modus); //underscore
		}
		else
		{
			display_util_write_char(43, inverse_modus); //unknown
		}
	}
}

void display_write_title(const char *text, uint8_t status)
{

	//text
	char *pointer = (char*) text;

	char blank = ' ';

	//counter and numbers
	uint8_t i = 0;
	uint8_t blank_number = 0;
	//counter to count the chars of the title
	uint8_t c = strlen(text);

	//prepare display
	display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);
	display_set_page(DISPLAY_PAGE_INIT);

	//Print left arrow if needed
	if (status & DISPLAY_TITLE_LEFT)
	{
		display_util_write_char(DISPLAY_CHAR_ARROW_LEFT, DISPLAY_MODE_INVERSE);
	}
	else
	{
		display_write_char(blank, DISPLAY_MODE_INVERSE);
	}

	blank_number = DISPLAY_CHAR_MAX - c - 2;

	//write title
	if (c <= DISPLAY_CHAR_MAX - 2)
	{

		for (i = 0; i < (blank_number / 2); i++)
		{
			display_write_char(blank, DISPLAY_MODE_INVERSE);
		}
		while (*pointer != '\0')
		{
			display_write_char(*pointer, DISPLAY_MODE_INVERSE);
			pointer++;
		}
		for (i = 0; i < ((blank_number / 2) + (blank_number % 2)); i++)
		{
			display_write_char(blank, DISPLAY_MODE_INVERSE);
		}
	}

	//error if title is too long
	else
	{
		display_write_title("Error", DISPLAY_TITLE_NONE);
		return;
	}

	//print right arrow if needed
	if (status & DISPLAY_TITLE_RIGHT)
	{
		display_util_write_char(DISPLAY_CHAR_ARROW_RIGHT, DISPLAY_MODE_INVERSE);
	}
	else
	{
		display_write_char(blank, 1);
	}

}

void display_write_text_status(uint8_t status)
{
	//Write the last display field
	//broken
	display_set_page(DISPLAY_PAGE_INIT + DISPLAY_PAGE_NUMBER);
	display_set_col(DISPLAY_COL_NUMBER_VISIBLE - DISPLAY_CHAR_WIDTH);

	//Paint navigation arrow if set
	if ((status & DISPLAY_TEXT_TOP) && (status & DISPLAY_TEXT_BOTTOM))
	{
		display_util_write_char(DISPLAY_CHAR_ARROW_TOP_AND_BOTTOM_SMALL,
				DISPLAY_MODE_NORMAL);
	}

	// Paint top arrow if set
	else if (status & DISPLAY_TEXT_TOP)
	{
		display_util_write_char(DISPLAY_CHAR_ARROW_TOP_SMALL,
				DISPLAY_MODE_NORMAL);
	}

	// Paint bottom arrow if set
	else if (status & DISPLAY_TEXT_BOTTOM)
	{
		display_util_write_char(DISPLAY_CHAR_ARROW_BOTTOM_SMALL,
				DISPLAY_MODE_NORMAL);
	}

	else
	{
		//do nothing
	}

}
;

void display_write_text(const char *text, uint8_t status)
{
	char *pointer = (char *) text;
	uint8_t i;
	uint8_t symbol = 0;
	static uint8_t row = 1;

	if (!(status & DISPLAY_TEXT_DEBUG))
	{
		//prepare display
		for (i = 1; i <= DISPLAY_PAGE_NUMBER; i++)
		{
			display_clean_line(i, 0);
		}
		row = 1;
	}

	display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);
	display_set_page(DISPLAY_PAGE_INIT + row);

	//start to write
	while (*pointer != '\0')
	{
		display_write_char(*pointer, DISPLAY_MODE_NORMAL);
		pointer++;
		symbol++;

		//line break
		if (symbol >= DISPLAY_CHAR_MAX)
		{
			symbol = 0;
			row++;

			//pagination
			if (row > DISPLAY_PAGE_NUMBER)
			{
				row = 1;
				if (!(status & DISPLAY_TEXT_DEBUG))
				{
					for (i = 1; i <= DISPLAY_PAGE_NUMBER; i++)
					{
						display_clean_line(i, DISPLAY_MODE_NORMAL);
					}
				}
			}

			display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);

			display_set_page(DISPLAY_PAGE_INIT + row);
		}
	}

	display_write_text_status(status);

	if (status & DISPLAY_TEXT_DEBUG)
	{
		row++;
		if (row > DISPLAY_PAGE_NUMBER)
		{
			row = 1;
		}
	}
}

void display_spinner(void)
{
	char *text = "Almond";
	static uint8_t tick = 0;

	//counter and numbers
	uint8_t i = 0;
	uint8_t symbol = 0;
	uint8_t row = 1;
	uint8_t blank_number = 0;
	//counter to count the chars of the title
	uint8_t c = strlen(text);

	blank_number = DISPLAY_CHAR_MAX - c;

	//prepare display
	display_clean();
	row = 2;

	display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);
	display_set_page(DISPLAY_PAGE_INIT + row);

	//Write Almond
	for (i = 0; i < (blank_number / 2); i++)
	{
		display_write_char(' ', DISPLAY_MODE_NORMAL);
	}
	while (*text != '\0')
	{
		display_write_char(*text, 0);
		text++;
		symbol++;
	}
	for (i = 0; i < ((blank_number / 2) + (blank_number % 2)); i++)
	{
		display_write_char(' ', DISPLAY_MODE_NORMAL);
	}

	// Paint dots
	row = 4;
	display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET + 5 * DISPLAY_CHAR_WIDTH);
	display_set_page(DISPLAY_PAGE_INIT + row);
	for (i = 0; i < 4; i++)
	{
		display_write_char(' ', DISPLAY_MODE_NORMAL);
		if (i != tick)
		{
			display_util_write_char(DISPLAY_CHAR_SPINNER_DOT,
					DISPLAY_MODE_NORMAL);
		}
		else
		{
			display_util_write_char(DISPLAY_CHAR_SPINNER_DOT_FULL,
					DISPLAY_MODE_NORMAL);
		}
	}

	tick = (tick + 1) % 4;

}
