#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "../drivers/display/display.h"

#define UART_BAUD_RATE 9600ul

/*! Define that selects the Usart used in example. */
#define USART USARTC0

void display_test(void);

int main(void)
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

	display_test();
}

void display_test(void)
{
	bool value = true;
	while(true){
		for(uint8_t y = 0; y < 64; y++){
			for(uint8_t x = 0; x < 128; x++){
				display_set_pixel(x,y,value);
			display_flip();
			}
		}
		//value = !value;
		_delay_ms(1000);
	}
}

