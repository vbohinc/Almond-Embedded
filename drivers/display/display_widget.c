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

//todo write status
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

	//TODO 1 in offset
	display_set_col(DISPLAY_COL_INIT + 1);
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

	//TODO 1 in offset
	display_set_col(DISPLAY_COL_INIT + 1);
	display_set_page(DISPLAY_PAGE_INIT + row);

	//time variables
	uint8_t year = 0;
	uint8_t month = 0;
	uint8_t day = 0;
	uint8_t hour = 0;
	uint8_t minute = 0;
	uint8_t second = 0;
	uint8_t leap = 0;

	uint8_t rem_secs = 0;

	//Get the year
	year = (time / (3 * DISPLAY_WIDGET_YEAR + 1 * DISPLAY_WIDGET_LEAP_YEAR))
			* 4;
	rem_secs = time % time / (3 * DISPLAY_WIDGET_YEAR + 1
			* DISPLAY_WIDGET_LEAP_YEAR);
	if (rem_secs > (2 * DISPLAY_WIDGET_YEAR + DISPLAY_WIDGET_LEAP_YEAR))
	{
		rem_secs = rem_secs % (2 * DISPLAY_WIDGET_YEAR
				+ DISPLAY_WIDGET_LEAP_YEAR);
		year += 3;
	}
	else if (rem_secs > (2 * DISPLAY_WIDGET_YEAR))
	{
		rem_secs = rem_secs % (2 * DISPLAY_WIDGET_YEAR);
		year += 2;
		leap = 1;
	}
	else if (rem_secs > (DISPLAY_WIDGET_YEAR))
	{
		rem_secs = rem_secs % (DISPLAY_WIDGET_YEAR);
		year++;
	}
	else
	{
		//nothing
	}

	//Get the month
	if (rem_secs > DISPLAY_WIDGET_TO_DECEMBER + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_DECEMBER+leap*24*60*60);
		month = 12;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_NOVEMBER + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_NOVEMBER+leap*24*60*60);
		month = 11;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_OCTOBER + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_OCTOBER+leap*24*60*60);
		month = 10;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_SEPTEMBER + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_SEPTEMBER+leap*24*60*60);
		month = 9;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_AUGUST + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_AUGUST+leap*24*60*60);
		month = 8;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_JULY + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_JULY+leap*24*60*60);
		month = 7;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_JUNE + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_JUNE+leap*24*60*60);
		month = 6;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_MAY + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_MAY+leap*24*60*60);
		month = 5;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_APRIL + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_APRIL+leap*24*60*60);
		month = 4;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_MARCH + leap*24*60*60)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_MARCH+leap*24*60*60);
		month = 3;
	}
	else if (rem_secs > DISPLAY_WIDGET_TO_FEBRUARY)
	{
		rem_secs = rem_secs - (DISPLAY_WIDGET_TO_FEBRUARY);
		month = 2;
	}
	else
	{
		month = 1;
	}

	//get days
	day = rem_secs/(86400); //24*60*60
	rem_secs = rem_secs%86400;

	//get hours
	hour= rem_secs/3600; //60*60
	rem_secs = rem_secs%3600;

	//Get minutes
	minute = rem_secs/60;
	rem_secs = rem_secs%60;

	//Get seconds
	second = rem_secs;


	sprintf(linebuffer, "Date");
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	sprintf(linebuffer, "%2d.%2d.%4d", day, month, year);
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);

	sprintf(linebuffer, "TIME");
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);
	sprintf(linebuffer, "%2d:%2d:%2d", hour, minute, second);
	display_write_text(linebuffer, status | DISPLAY_TEXT_DEBUG);

}

