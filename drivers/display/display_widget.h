#ifndef __DISPLAY_WIDGET_H__
#define __DISPLAY_WIDGET_H__
/**
 * Widget to show wind direction and wind force
 * @param direction direction of the wind
 * @param force force of the wind
 */
void display_widget_wind(uint8_t direction, uint8_t force, uint8_t status);

/**
 * Widget to show temperature, air humidity, air pressure
 * @param temperature temperature
 * @param humidity
 * @param pressure
 */
void display_widget_weather(uint8_t temperature, uint8_t humidiy, uint8_t pressure, uint8_t status);

/**
 * Widget to show intensity of light
 * @param intensity intensity of the light
 */
void display_widget_light(uint8_t intensity, uint8_t status);


#endif //__DISPLAY_WIDGET_H__
