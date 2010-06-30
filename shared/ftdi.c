/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
      ___           ___           ___           ___           ___           ___     
     /  /\         /  /\         /__/\         /__/\         /  /\         /__/\    
    /  /:/        /  /::\       |  |::\       |  |::\       /  /::\        \  \:\   
   /  /:/        /  /:/\:\      |  |:|:\      |  |:|:\     /  /:/\:\        \  \:\  
  /  /:/  ___   /  /:/  \:\   __|__|:|\:\   __|__|:|\:\   /  /:/  \:\   _____\__\:\ 
 /__/:/  /  /\ /__/:/ \__\:\ /__/::::| \:\ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\
 \  \:\ /  /:/ \  \:\ /  /:/ \  \:\~~\__\/ \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/
  \  \:\  /:/   \  \:\  /:/   \  \:\        \  \:\        \  \:\  /:/   \  \:\  ~~~ 
   \  \:\/:/     \  \:\/:/     \  \:\        \  \:\        \  \:\/:/     \  \:\     
    \  \::/       \  \::/       \  \:\        \  \:\        \  \::/       \  \:\    
     \__\/         \__\/         \__\/         \__\/         \__\/         \__\/    


#include "ftdi.h"

void FTDIInit(void)
{
#ifndef __AVR_ATxmega128A1__
	// initialize data direction
	DDRB |= FTDI_DD;
	DDRA = 0xFF;

	PORTB |= (1<<P_SI);
#endif
}

void FTDISend( uint8_t out_buf)
{
#ifndef __AVR_ATxmega128A1__
	if (out_buf == '\0')
	{
		FTDISendImmediate();
		return;
	}

	while( TXE !=0 ) {};

	PORTA = out_buf;
	PORTB |= (1<<P_WR);
	nop(); nop();
	PORTB &= ~(1<<P_WR);
#endif
}

void FTDISendImmediate(void)
{
#ifndef __AVR_ATxmega128A1__
	// strobe low
	PORTB &= ~(1<<P_SI);
	PORTB |= (1<<P_SI);
#endif
}

uint8_t FTDIRead( uint8_t *out_buf)
{
#ifndef __AVR_ATxmega128A1__
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


#endif
	return 1;
}
