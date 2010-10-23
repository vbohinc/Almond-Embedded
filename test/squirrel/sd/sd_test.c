#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE      9600ul

#include "common.h"
#include <drivers/spi/spi.h>
#include <drivers/storage/sd.h>

#define USART USARTC0


int main (void)
{

	/* Behold the MAGIC Clock! */

	/* Internen 32Mhz Oszillator einschalten */
	OSC.CTRL = OSC_RC32MEN_bm;

	/* Warten bis Oszillator stabil ist */
	while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);

	/* System Clock selection */
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;

	/* DFLL ein (Auto Kalibrierung) */
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;


	set_bit(PORTC.DIR,4);
	clear_bit(PORTC.OUT,4);

	set_bit(PORTH.DIR,2);
	set_bit(PORTH.OUT,2);

	spi_init();

	sd_init();

	while (1)
	{
		
	}
}
