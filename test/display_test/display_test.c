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


int main(void)
{
	display_init();
	//display_test2();
	display_draw_char(10,10,0,'!');
	display_flip();
	
}

