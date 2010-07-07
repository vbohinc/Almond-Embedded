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
	//todo real
	static uint8_t max_symbols = 40;

	char linebuffer[max_symbols+1];
	linebuffer[max_symbols]= '\0';
	const char *text_1 = "Temperature: ";
	const char *text_2 = "Humidity: ";
	const char *text_3 = "Pressure: ";

	//TODO check overflow
	char text_val_1 [4];
	char text_val_2 [4];
	char text_val_3 [4];

	itoa(temperature, text_val_1, 10);

	//prepare display
	for (i = 1; i <= DISPLAY_PAGE_NUMBER; i++)
	{
		display_clean_line(i, 0);
	}
	//TODO 1 in offset
	display_set_col(DISPLAY_COL_INIT + 1);
	display_set_page(DISPLAY_PAGE_INIT + row);


	display_write_text(linebuffer, status|DISPLAY_TEXT_DEBUG);
	display_write_text(linebuffer, status|DISPLAY_TEXT_DEBUG);
	display_write_text(linebuffer, status|DISPLAY_TEXT_DEBUG);
	display_write_text(linebuffer, status|DISPLAY_TEXT_DEBUG);



}

//TODO write
void display_widget_light(uint8_t intensity, uint8_t status) {
	const char *text;
	text = "light widget";
	display_write_text(text, 0);
}

//TODO write
void display_widget_time(uint32_t time, uint8_t status)  {
	const char *text;
	text = "time widget";
	display_write_text(text, 0);

}
