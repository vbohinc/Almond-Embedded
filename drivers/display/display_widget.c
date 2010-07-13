#include "display_widget.h"
#include "display.h"
#include <stdio.h>
#include <string.h>
//TODO Find a way to remove this include
#include "display_low.h"
//TODO write
void display_widget_wind(uint8_t direction, uint8_t force, uint8_t status) {
	const char *text;
	text = "wind widget";
	display_write_text(text, 0);
}

//TODO write
void display_widget_weather(uint16_t temperature, uint16_t humidity, uint16_t pressure, uint8_t status) {

	//Counter
	uint8_t row = 1;
	uint8_t i;

	//TODO check overflow?
	char linebuffer[DISPLAY_CHAR_MAX+1];


	char minibuffer[4];


	const char *text_1 = "Temperature: ";
	const char *text_2 = "Humidity:    ";
	const char *text_3 = "Pressure:    ";

	char text_val_1 [DISPLAY_CHAR_MAX+1];
	char text_val_2 [DISPLAY_CHAR_MAX+1];
	char text_val_3 [DISPLAY_CHAR_MAX+1];

	sprintf(text_val_1,"%d", temperature);
	sprintf(text_val_2,"%d", humidity);
	sprintf(text_val_3,"%d", pressure);


	//prepare display
	for (i = 1; i <= DISPLAY_PAGE_NUMBER; i++)
	{
		display_clean_line(i, 0);
	}

	//TODO 1 in offset
	display_set_col(DISPLAY_COL_INIT + 1);
	display_set_page(DISPLAY_PAGE_INIT + row);


	display_write_text("", status|DISPLAY_TEXT_DEBUG);
	linebuffer[0]= '\0';
	minibuffer[3]= '\0';
	strcat(linebuffer, text_1);
	strncpy(minibuffer,text_val_1,2);
	strncat(linebuffer, minibuffer,2);
	minibuffer[0]='.';
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	strncpy(minibuffer,&text_val_1[2],1);
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	minibuffer[0]='~'; //centigrade
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	display_write_text(linebuffer, status|DISPLAY_TEXT_DEBUG);

	display_write_text("", status|DISPLAY_TEXT_DEBUG);
	linebuffer[0]= '\0';
	minibuffer[3]= '\0';
	strcat(linebuffer, text_2);
	strncpy(minibuffer,text_val_2,2);
	strncat(linebuffer, minibuffer,2);
	minibuffer[0]='.';
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	strncpy(minibuffer,&text_val_2[2],1);
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	minibuffer[0]='%';
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	display_write_text(linebuffer, status|DISPLAY_TEXT_DEBUG);

	display_write_text("", status|DISPLAY_TEXT_DEBUG);
	linebuffer[0]= '\0';
	minibuffer[3]= '\0';
	strcat(linebuffer, text_3);
	strncpy(minibuffer,text_val_3,2);
	strncat(linebuffer, minibuffer,2);
	minibuffer[0]='.';
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
	strncpy(minibuffer,&text_val_3[2],1);
	minibuffer[1]='\0';
	strcat(linebuffer, minibuffer);
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

