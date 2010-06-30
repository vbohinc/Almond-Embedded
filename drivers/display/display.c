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

const uint8_t charray[][6] =
{
//0
		{ 0x0, 0x3e, 0x45, 0x49, 0x51, 0x3e },
		//1
		{ 0x0, 0x0, 0x10, 0x20, 0x7f, 0x0 },
		//2
		{ 0x0, 0x21, 0x43, 0x45, 0x49, 0x31 },
		//3
		{ 0x0, 0x41, 0x49, 0x49, 0x7f, 0x0 },
		//4  improve
		{ 0x0, 0x7c, 0x4, 0x4, 0xf, 0x4 },
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
		//16:G improve
		{ 0x0, 0x7f, 0x41, 0x4d, 0x49, 0x4f },
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
		//Q improve
		{ 0x0, 0x3c, 0x42, 0x46, 0x42, 0x3d },
		//R  improve?
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
		//boarder: vertical left
		{ 0xff, 0x0, 0x0, 0xff, 0x0, 0x0 },
		//boarder: vertical right
		{ 0x0, 0x0, 0xff, 0x0, 0x0, 0xff },
		//boarder: horizontal top
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 },
		//boarder: horizontal bottom
		{ 0x9, 0x9, 0x9, 0x9, 0x9, 0x9 },
		//40:boarder: top left
		{ 0xff, 0x80, 0x80, 0x9f, 0x90, 0x90 },
		//boarder: top right
		{ 0x90, 0x90, 0x9f, 0x80, 0x80, 0xff },
		//boarder: bottom left
		{ 0xff, 0x1, 0x1, 0xf9, 0x9, 0x1 },
		//boarder: bottom right
		{ 0x9, 0x9, 0xf9, 0x1, 0x1, 0xff },
		//centigrade
		{ 0x0, 0x0, 0x0, 0x70, 0x50, 0x70 },
		//45:. dot
		{ 0x0, 0x0, 0x0, 0x3, 0x3, 0x0 },
		//: double dot
		{ 0x0, 0x0, 0x0, 0x33, 0x33, 0x0 },
		//, semicolon
		{ 0x0, 0x4, 0x7, 0x0, 0x0, 0x0 },
		//_ underline
		{ 0x0, 0x1, 0x1, 0x1, 0x1, 0x1 },
		//- minus
		{ 0x0, 0x8, 0x8, 0x8, 0x8, 0x8 },
		//50: + plus
		{ 0x0, 0x8, 0x8, 0x3e, 0x8, 0x8 },
		// unknown full
		{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
		// 52:blank
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

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
	//-----------------------------------------
	//Set reference voltage mode
	display_command(0x81);
	//Set reference voltage register
	display_command(0x30);
	//------------------------------------
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

	/*
	 Not needed because display_clean()
	 //Set initial column and page
	 display_set_col(DISPLAY_COL_INIT);
	 display_set_page(DISPLAY_PAGE_INIT);
	 */

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
			display_write(0x00, 0);
		}

	}
	display_set_col(DISPLAY_COL_INIT);
	display_set_page(DISPLAY_PAGE_INIT);
}

/**
 * help function in order to write a char
 */

static void display_write_char_util(uint8_t number, uint8_t inverse)
{
	int i;
	for (i = 0; i < 6; i++)
	{
		display_write(charray[number][i], inverse);
	}
}

void display_write_char(uint8_t *character, uint8_t inverse)
{
	if (*character != '\0')
	{
		if (*character >= 'A' && *character <= 'Z')
		{
			display_write_char_util(*character - 55, inverse); // A-Z
		}
		else if (*character >= 'a' && *character <= 'z')
		{
			display_write_char_util(*character - 87, inverse); //A-Z
		}
		else if (*character >= '0' && *character <= '9')
		{
			display_write_char_util(*character - 48, inverse); //0-9
		}
		else if (*character == ' ')
		{
			display_write_char_util(52, inverse); //blank
		}
		else if (*character == '.')
		{
			display_write_char_util(45, inverse); //dot
		}
		else if (*character == ':')
		{
			display_write_char_util(46, inverse); //double dot
		}
		else
		{
			display_write_char_util(51, inverse); //unknown
		}
	}
}

void display_write_text(char *text, uint8_t status)
{

	uint8_t *pointer = (uint8_t *) text;
	static uint8_t col = 0;
	static uint8_t row = 1;
	static uint8_t max_row = DISPLAY_COL_NUMBER_VISIBLE / DISPLAY_CHAR_WIDTH;

	//prepare display
	display_clean();
	display_set_col(DISPLAY_COL_INIT + 1);
	display_set_page(DISPLAY_PAGE_INIT + 1);

	//start to write
	while (*pointer != '\0')
	{

		//pagination
		if (row > DISPLAY_PAGE_NUMBER)
		{
			row = 1;
			//TODO remove clean in order to save the titlebar
			display_clean();
			display_set_col(DISPLAY_COL_INIT + 1);
		}

		//Let the last space free for GUI Element
		if ((row == DISPLAY_PAGE_NUMBER) && col == max_row - 2
				&& (check_bit(status,7) || (check_bit(status,0))))
		{
			col = max_row - 1;
		}

		display_write_char(pointer, 0);
		pointer++;
		col++;

		if (col >= max_row)
		{
			col = 0;
			display_set_col(DISPLAY_COL_INIT + 1);
			row++;
			display_set_page(DISPLAY_PAGE_INIT + row);
		}

	}
	//TODO Painting the top/bottom symbols

}

