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

int main(void)
{
	FTDIInit();
	DDRD |= 0xFF;

	DDRC |= 0xFF;

	FTDISend('H');
	FTDISend('a');
	FTDISend('l');
	FTDISend('l');
	FTDISend('o');
	FTDISend('1');


	while (1)
	{

		PORTD |= (1<<7);
		PORTC |= (1<<0);
		_delay_ms(500);
		PORTD &= ~(1<<7);
		PORTC &= ~(1<<0);
		_delay_ms(500);

		bluetooth_init(bluetooth_callback_handler);

		PORTD |= (1<<7);
		_delay_ms(100);
		PORTD &= ~(1<<7);
		_delay_ms(100);

		int ret = bluetooth_test_connection(4);
		if (ret == 0)
		{
			FTDISend('N');
			FTDISend('O');
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
		}

		_delay_ms(2000);
	}
}
