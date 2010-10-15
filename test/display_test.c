#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../drivers/display/display.h"

#define UART_BAUD_RATE      9600ul

/*! Define that selects the Usart used in example. */
#define USART USARTC0

int main(void)
{
	display_test();
}

void display_test(void)
{	
	bool value = false;
	while(true){
		for(uint8_t y = 0; y <= 64; y++){
			for(uint8_t x = 0; x <= 128; x++){
				display_set_pixel(x,y,value);
			}
		}
		value = !value;
	}
}