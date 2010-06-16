/*
 * main.c
 *
 *  Created on: 02.05.2010
 *      Author: stefan
 */

#include <avr/io.h>
#include <util/delay.h>
#include "drivers/bluetooth/bluetooth.h"
#include "shared/ftdi.h"

void bluetooth_callback_handler(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{

}

void print(char * arr){
	char* c = arr;
	while ((*c)!=0)
	{
		FTDISend(*c);
		c++;
	}
	FTDISend(0);

}

int main(void)
{
	FTDIInit();
	DDRD |= 0xFF;

	DDRC |= 0xFF;

	PORTD |= (1<<7);
			PORTC |= (1<<0);
			_delay_ms(500);
			PORTD &= ~(1<<7);
			PORTC &= ~(1<<0);
			_delay_ms(500);


			FTDISend(10);
			FTDISend(13);
			FTDISend('#');
			FTDISend('#');
			FTDISend('#');
			FTDISend(10);
			FTDISend(13);

	//for (uint8_t i=0; i<10 && hallo[i]!=0; i++)
	//	FTDISend(hallo[i]);
	PORTD |= (1<<7);
			PORTC |= (1<<0);
			_delay_ms(500);
			PORTD &= ~(1<<7);
			PORTC &= ~(1<<0);
			_delay_ms(500);

	bluetooth_init(bluetooth_callback_handler);

	sei();

	while (1)
	{

		PORTD |= (1<<7);
		PORTC |= (1<<0);
		_delay_ms(500);
		PORTD &= ~(1<<7);
		PORTC &= ~(1<<0);
		_delay_ms(500);

		int ret = bluetooth_test_connection(4);
		if (ret == 0)
		{
			FTDISend('N');
			FTDISend('O');

	    	FTDISend(13);
	    	FTDISend(10);
			PORTD |= (1<<7);
			_delay_ms(500);
			PORTD &= ~(1<<7);
			_delay_ms(500);
			PORTD |= (1<<7);
			_delay_ms(500);
			PORTD &= ~(1<<7);
		}
		else
		{

			FTDISend('O');
			FTDISend('K');
			PORTC |= (1<<0);
			_delay_ms(500);
			PORTC &= ~(1<<0);
			_delay_ms(500);
			PORTC |= (1<<0);
			_delay_ms(500);
			PORTC &= ~(1<<0);

			char* ret = bluetooth_cmd_get_address();
			for (uint8_t i=0; ret[i]!='\0'; i++)
			{
				FTDISend(ret[i]);
			}
		}

		_delay_ms(2000);
	}
}
