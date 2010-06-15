#include "ftdi.h"

void FTDIInit(void)
{
	// initialize data direction
	DDRB |= FTDI_DD;
	DDRA = 0xFF;

	PORTB |= (1<<P_SI);
}

void FTDISend( uint8_t out_buf)
{
	while( TXE !=0 ) {};

	PORTA = out_buf;
	PORTB |= (1<<P_WR);
	nop(); nop();
	PORTB &= ~(1<<P_WR);
}

void FTDISendImmediate(void)
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
	DDRA = 0x00;

	PORTB &= ~(1<<P_RD);
	PORTB |= (1<<P_RD);

	// read byte out
	(*out_buf) = PINA;

	// set direction
	DDRA = 0xFF;

	return 1;
}
