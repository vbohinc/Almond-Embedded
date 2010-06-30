#include "display_widget.h"
#include "display.h"

//TODO write
void display_widget_wind(uint8_t direction, uint8_t force, uint8_t status) {
	const char *text;
	text = "wind widget";
	display_write_text(text, 0);
}

//TODO write
void display_widget_weather(uint8_t temperature, uint8_t humidiy, uint8_t pressure, uint8_t status) {
	const char *text;
	text = "weather widget";
	display_write_text(text, 0);
}

//TODO write
void display_widget_light(uint8_t intensity, uint8_t status) {
	const char *text;
	text = "light widget";
	display_write_text(text, 0);
}
