#include "error_driver.h"

#ifdef DEBUG

#ifdef DEBUG_TO_DISPLAY
#include <display/display.h>
#endif

#ifdef __AVR_ATmega8535__
	#include "ftdi.h"
	void error_driver_write_c(uint8_t c)
	{
		FTDISend(c);
	}

	inline void error_driver_init(void)
	{
		FTDIInit();
	}
#elif defined(__AVR_ATxmega128A1__)

	#include "../drivers/uart/usart_driver.h"

	/*! Define that selects the Usart used in example. */
	#define USART USARTC1

	void error_driver_write_c(uint8_t c)
	{
		#ifdef DEBUG_TO_DISPLAY
		char s[] = {c,'\0'};
		display_print(s);
		display_flip();
		#endif
		
		#ifndef DEBUG_TO_DISPLAY
		/* Send one char. */
				do{
		/* Wait until it is possible to put data into TX data register.
		 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
		}while(!USART_IsTXDataRegisterEmpty(&USART));
		USART_PutChar(&USART, c);
		#endif
	}

	inline void error_driver_init(void)
	{
		#ifndef DEBUG_TO_DISPLAY
		/* This PORT setting is only valid to USARTC0 if other USARTs is used a
		 * different PORT and/or pins is used. */
		/* PIN3 (TXD0) as output. */
		PORTC.DIRSET = PIN7_bm;

		/* PC2 (RXD0) as input. */
		PORTC.DIRCLR = PIN6_bm;

		/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
		USART_Format_Set(&USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);

		/* Set Baudrate to 9600 bps:
		 * Use the default I/O clock fequency that is 2 MHz.
		 * Do not use the baudrate scale factor
		 *
		 * Baudrate select = (1/(16*(((I/O clock frequency)/Baudrate)-1)
		 *                 = 12
		 */
		USART_Baudrate_Set(&USART, 212/*12*/ , 0);

		/* Enable both RX and TX. */
		USART_Rx_Enable(&USART);
		USART_Tx_Enable(&USART);
		#endif
	}

#else
#error "No definition for error_driver available"
#endif
#endif
