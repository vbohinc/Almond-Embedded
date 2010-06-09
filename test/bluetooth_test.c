/*
 * main.c
 *
 *  Created on: 02.05.2010
 *      Author: stefan
 */

#include <avr/io.h>
#include <util/delay.h>
#include "drivers/bluetooth/bluetooth.h"

#define nop() \
   asm volatile ("nop");

#define P_RD  PB2
#define P_WR  PB3
#define P_TXE PB0
#define P_RXF PB1
#define P_SI  PB4

#define DD_RD  DDB2 // 1
#define DD_WR  DDB3 // 1
#define DD_TXE DDB0 // 0
#define DD_RXF DDB1 // 0
#define DD_SI  DDB4 // 1

#define TXE (PINB & (1<<P_TXE))
#define RXF (PINB & (1<<P_RXF))

#define FTDI_DD ( (1 << DD_RD) | (1 << DD_WR) | (1 << DD_SI) )

void FTDIInit(void)
{
	// initialize data direction
	DDRB |= FTDI_DD;
	DDRD = 0xFF;

	PORTB |= (1<<P_SI);
}

void FTDISend( uint8_t out_buf)
{
	while( TXE !=0 ) {};

	PORTD = out_buf;
	PORTB |= (1<<P_WR);
	nop(); nop();
	PORTB &= ~(1<<P_WR);
}

void FTDISendImmediate()
{
	// strobe low
	PORTB &= ~(1<<P_SI);
	PORTB |= (1<<P_SI);
}

uint8_t FTDIRead( uint8_t *out_buf)
{
	// no data avaiable
	if( RXF)
		return 0;

	// set direction
	DDRD = 0x00;

	PORTB &= ~(1<<P_RD);
	PORTB |= (1<<P_RD);

	// read byte out
	(*out_buf) = PIND;

	// set direction
	DDRD = 0xFF;

	return 1;
}


void bluetooth_callback_handler(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{

}

int main(void)
{
	FTDIInit();
	DDRD |= 0xFF;

	DDRC |= 0xFF;

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
