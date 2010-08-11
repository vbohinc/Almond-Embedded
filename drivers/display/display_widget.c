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

#include "display_widget.h"
#include "display.h"
#include <stdio.h>
#include <string.h>
//TODO Find a way to remove this include
#include "display_low.h"

int16_t abs(int16_t i)
{
	if (i < 0)
	{
		return -i;
	}
	return i;
}

//TODO write
void display_widget_wind(uint8_t direction, uint8_t force, uint8_t status)
{
	const char *text;
	text = "wind widget";
	display_write_text(text, 0);
}


void display_widget_weather(int16_t temperature, uint16_t humidity,
		uint16_t pressure, uint8_t status)
{

	//Counter
	uint8_t row = 1;
	uint8_t i;

	//TODO check overflow?
	char linebuffer[DISPLAY_CHAR_MAX + 1];

	//prepare display
	for (i = 1; i <= DISPLAY_PAGE_NUMBER; i++)
	{
		display_clean_line(i, 0);
	}

	display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);
	display_set_page(DISPLAY_PAGE_INIT + row);

	if (temperature > -10000 && temperature < 10000)
	{
		sprintf(linebuffer, "Temperature");
		display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
		sprintf(linebuffer, "%+3d.%2d C°", temperature / 100, abs(temperature)
				% 100);
		display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	}
	if (humidity > 0 && humidity < 200)
	{
		sprintf(linebuffer, "Humidity");
		display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
		sprintf(linebuffer, "%3d %%", humidity);
		display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	}
	if (pressure >= 300 && pressure <= 1100)
	{
		sprintf(linebuffer, "Pressure");
		display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
		sprintf(linebuffer, "%4d hPa", pressure);
		display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	}

}

//TODO write
void display_widget_light(uint8_t intensity, uint8_t status)
{
	const char *text;
	text = "light widget";
	display_write_text(text, 0);
}

//TODO write
void display_widget_time(uint32_t time, uint8_t status)
{

	//Counter
	uint8_t row = 1;
	uint8_t i;

	//TODO check overflow?
	char linebuffer[DISPLAY_CHAR_MAX + 1];

	//prepare display
	for (i = 1; i <= DISPLAY_PAGE_NUMBER; i++)
	{
		display_clean_line(i, 0);
	}

	display_set_col(DISPLAY_COL_INIT + DISPLAY_COL_OFFSET);
	display_set_page(DISPLAY_PAGE_INIT + row);




	sprintf(linebuffer, "Date");
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	//sprintf(linebuffer, "%2d.%2d.%4d", day, month, year);
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);

	sprintf(linebuffer, "Time");
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	//sprintf(linebuffer, "%2d:%2d:%2d", hour, minute, second);
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);

}

