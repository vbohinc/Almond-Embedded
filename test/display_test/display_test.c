#ifndef X86
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

#include "../../drivers/display/display.h"
#include "../../drivers/display/display_draw.h"

#define UART_BAUD_RATE      9600ul

/*! Define that selects the Usart used in example. */
#define USART USARTC0

void display_test2(void)
{	
	bool value = true;
	for(int i = 0; i < 10;i++){
		for(uint8_t y = 0; y <= 64; y++){
			for(uint8_t x = 0; x <= 128; x++){
				display_set_pixel(x,y,value);
				display_flip();
			}
		}
		value = !value;
	}
}

#include <stdbool.h>
#include "SDL.h" // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

const uint8_t testimg1[] 
#ifndef X86
PROGMEM
#endif
= {
	128, 64
	,  9, 81,103, 64,  0,  0,  0,  0, 12,  0,  0,  0,  0,  0,  0,  0,102, 90,122,128,  0
	,  0,  0,  0,  2, 32,  0,  0,  0,  0,  0,  0, 41, 37,157,128,  0,  0,  0,  0, 23,  0
	,  0,  0,  0,  0,  0,  0,170,146,177, 32,  0,  0,  0,  0, 73,  0,  0,  0,  0,  0,  0
	,  0, 69, 45, 87,  0,  0,  0,  0,  0, 64,192,  0,  0,  0,  0,  0,  0,166,202,200, 32
	,  0,  0,  0,  0, 36,  0,  0,  0,  0,  0,  0,  0,170,155,246,  0,  0,  0,  0,  0, 36
	,  0,  0,  0,  0,  0,  0,  0, 23, 87, 92,  0,  0,  0,  0,  0, 27,128,  0,  0,  0,  0
	,  0,  0,221,175,117, 32,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0,  0, 87, 93,232
	,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,171,206,212,  0,  0,  0,  0,  0
	,  0,  0,  0,  0,  0,  0,  0,  0, 91,187,160,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
	,  0,  0,  0,175,251,229,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,183,223
	,144,  0,  0,  0,  0,  0,  0,  8,  8,  0,  0,  0,  0,  0,155,127, 72,  0,  0,  0,  0
	,  0,  0, 18,128,  0,  0,  0,  0,  0,119,253, 18,  0,  0,  0,  0,  0,  0,  4,  0,  0
	,  0,  0,  0,  9,159,238, 72,  0,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  5,223
	,254, 64,  0,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,  0,175,252,128,  0,  0,  0
	,  0,  0,  0,  8,  0,  0,  0,  0,  0,  1,111,235, 37,  0,  0,  0,  0,  0,  0, 17,  0
	,  0,  0,  0,  0,  1,159,218,  0,  0,  0,  0,  0,  0,  0,128,192,  1,  0,  0,  0,  0
	,127,242,160,  0,  0,  0,  0,  0,  0, 16,  8,  0,  0,  0,  0,  0,159,164,  0,  0,  0
	,  0,  0,  0,  0,  9,  0,  0,  0,  0,  0,  0,255,233, 80,  0,  0,  0,  0,  0,  0, 33
	,  0, 16,  0,  0,  0,  0, 95, 85,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0
	,  0, 95,162,160,  0,  0,  0,  0,  0,  1,144,161,  0,  0,  0,  0,  0,191,172,  2,128
	,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,127,209, 26,128,  0,  0,  0,  0,  0
	, 70, 34,  0,  0,  0,  0,  0,223, 52,251,  0,  0,  0,  0,  0,  0,  0,128,  0,  0,  0
	,  0,  0, 63,108,249, 32,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,239,211,237
	,128,  0,  0,  0,  0,  0, 66,160,  0,  0,  0,  0,  0,126,179,254,  0,  0,  0,  0,  0
	,  0,  0,  0,  0,  0,  0,  0,128,127, 77,243, 16,  0,  0,  0,  0,  0, 16,  0,  0,  0
	,  0,  0,  0,221,179,241,200,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  1,128,125,231
	,248,160,  0,  0,  0,  0,  0, 32,  0,  0,  0,  0,  0,128,191, 91,247, 64,  0,  0,  0
	,  0,  0,  4,  0,  0,  0,  0,  2, 24,189,231,255,104,  0,  0,  0,  0,  0, 17,  0,  0
	,  0,  0,  2, 24,255,175,255,176,  0,  0,  0,  0,  0,  0,  0, 64,  0,  0,  1,188,111
	, 81,255,224,  0,  0,  0,  0,128,  8,128,  0,  0,  0,  0, 62,255,123,255, 64,  8,  0
	,  0,  0,  0, 20,  0,  0,  0,  0,  0,156,111,201,191,144,  0,  0,  0,  0, 64,  1, 32
	, 16,  0,  0,  1,  4,255,237,254,192,  0,  0,  0,  0,  0,  2,  8,128,  0,  0,  2, 98
	,127,190,255,144,  0,  0,  0,  0,  0,  1, 80,  2,  0,  0,  0,200,255,243,108,  0,  0
	,  0,  0,  4,  0, 20,132,  0, 64,  0,  1, 32,254,221,146,144,  0,  0,  0,  8,  0,  1
	, 81, 69,  0,  0,  0, 80,127,213,109, 34,  0,  0,  0, 32,  0,  2,160, 80, 32,  0,  0
	,  4,239,239,104,168,  0,  0,  0, 64,  0,  0, 89,132, 16,  2,  4, 32,191,187,174, 80
	,128,  0,  1,136,  0,  1, 84,144,  0,128,  0,  0,255,254,243, 98, 16,  0,  0,144,  0
	,  4, 54, 42,160, 32,  0,  0, 95,247, 77,  8,  0, 68,  5,  0,  0,  0,186,145,  1,  1
	,  0,  0,223,255,250,169, 68,  1, 10,128,  0,  2, 53,100, 80, 16,  0,  0,191,255,175
	, 36, 16,  2,101, 32,  0,  0, 75,147, 33,  1, 72,  1,255,251,250,179, 65, 16, 24,  0
	,  0,  2, 13,236,180,168,  0,  9,159,251,174,204,163, 37,186,  0,  0,  0,150,235, 83
	, 18,136, 19,255,253,255, 85, 88,153, 80,  0,  0,  9, 11,122, 68,160, 34, 39,255,254
	,251,103, 23,119,228, 16,  0,  0, 17,221,147, 12,164, 95,111,255,253,212,233, 63,224
	,136,  0, 17, 68,127,105,160,180,191,223,255,255,127,174,238,  0,  0,  0,  4, 17,127
	,180, 83, 25,127,127,255,255,255,245,246,168,  0,  0,  2, 32,143,219,164,101,255,219
	,255,255,250,255,244, 64,  0,  0,  0,  4, 51,249,103,167,127,127,255,191,255,255,236
	,  0,128,  0,  0, 33,  6,255, 91,255,255,247,255,239,255,255, 32, 72,  0,  0,  0,132
	, 83,127,254, 31,191, 95,255,211,255,253, 68, 33,  0,  4,  0, 73,  4,155,232, 63,255
	,255,255,253, 86,196,144,  0,  0,  0,  0,  0, 16,  0,  0, 31,255
};


int main(void)
{
	display_init();
	//display_test2();
//	display_draw_string(10,10,2,"Hallo");
	display_draw_image(0,0,(uint8_t*)testimg1);
//	display_draw_char(10,10,0,'#');
	display_flip();
	
}

