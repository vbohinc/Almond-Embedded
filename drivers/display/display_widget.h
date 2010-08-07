#include <avr/io.h>

#ifndef __DISPLAY_WIDGET_H__
#define __DISPLAY_WIDGET_H__
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
