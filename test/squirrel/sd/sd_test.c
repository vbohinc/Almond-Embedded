#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE      9600ul

#include <common.h>
#include "../../../drivers/storage/sd.h"
#include "../../../drivers/display/display.h"

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

	display_init();
	button_init();

	debug("-- SD Test --\n");
	display_flip();
	sd_init();
	debug("SD Initialized\n");
	display_flip();	
	



	uint8_t arr[5] = {1,2,3,4,5};



	uint8_t arr_ret[5];

	if (!sd_write_bytes(0,arr,sizeof(arr)))
		error("Couldn't write\n");

	display_flip();
	if (!sd_read_bytes(0,arr_ret,sizeof(arr_ret)))
		error("Couldn't read\n");

	display_flip();
	for (uint8_t i=0; i<5; i++)
	{
		debug("Read: ");
		error_putc('0'+arr_ret[i]);
		//display_print("\n");
		display_flip();
	}
	while (1)
	{
		
	}
}
