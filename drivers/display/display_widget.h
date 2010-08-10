#include <avr/io.h>

#ifndef __DISPLAY_WIDGET_H__
#define __DISPLAY_WIDGET_H__


#define DISPLAY_WIDGET_YEAR 31536000//(365*24*60*60)
#define DISPLAY_WIDGET_LEAP_YEAR 31622400//(366*24*60*60)

#define DISPLAY_WIDGET_TO_DECEMBER 		28857600			//((31+28+31+30+31+30+31+31+30+31+30)*24*60*60) = 334 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_NOVEMBER		26265600			//((31+28+31+30+31+30+31+31+30+31)*24*60*60) = 304 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_OCTOBER 		23587200			//((31+28+31+30+31+30+31+31+30)*24*60*60) = 273 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_SEPTEMBER 	20995200			//((31+28+31+30+31+30+31+31)*24*60*60) = 243 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_AUGUST 		18316800			//((31+28+31+30+31+30+31)*24*60*60) = 212 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_JULY 			15638400			//((31+28+31+30+31+30)*24*60*60) = 181 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_JUNE 			13046400			//((31+28+31+30+31)*24*60*60) = 151 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_MAY 			10368000			//((31+28+31+30)*24*60*60) = 120 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_APRIL 		7776000				//((31+28+31)*24*60*60) = 90 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_MARCH 		5097600				//((31+28)*24*60*60) = 59 * 24 * 60 *60
#define DISPLAY_WIDGET_TO_FEBRUARY 		2678400				//((31)*24*60*60) = 31 * 24 * 60 *60

/**
 * Widget to show wind direction and wind force
 * @param direction direction of the wind
 * @param force force of the wind
 * @param status of the title
 */
void display_widget_wind(uint8_t direction, uint8_t force, uint8_t status);

/**
 * Widget to show temperature, air humidity, air pressure
 * @param temperature temperature
 * @param humidity
 * @param pressure
 * @param status of the title
 */
void display_widget_weather(int16_t temperature, uint16_t humidity, uint16_t pressure, uint8_t status);

/**
 * Widget to show intensity of light
 * @param intensity intensity of the light
 * @param status of the title
 */
void display_widget_light(uint8_t intensity, uint8_t status);

/**
 * Widget to show time
 * @param time timestamp unix format
 * @param status of the title
 */
void display_widget_time(uint32_t time, uint8_t status);



#endif //__DISPLAY_WIDGET_H__
