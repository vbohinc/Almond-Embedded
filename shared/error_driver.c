#include "error_driver.h"

#ifdef DEBUG


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

	#define UART0_RECEIVE_INTERRUPT   USARTD0_RXC_vect
	#define UART0_TRANSMIT_INTERRUPT  USARTD0_DRE_vect

	/*! Input data buffer for interrupt on receive */
	USART_data_t USART_data;

	/*! Define that selects the Usart used in example. */
	#define USART USARTD0

	void error_driver_write_c(uint8_t c)
	{
		USART_TXBuffer_PutByte(&USART_data,c);
	}

	inline void error_driver_init(void)
	{
		/* This PORT setting is only valid to USARTC0 if other USARTs is used a
		 * different PORT and/or pins is used. */
		/* PIN3 (TXD0) as output. */
		PORTD.DIRSET = PIN3_bm;

		/* PC2 (RXD0) as input. */
		PORTD.DIRCLR = PIN2_bm;


		/* Use USARTC0 and initialize buffers. */
		USART_InterruptDriver_Initialize(&USART_data, &USART, USART_DREINTLVL_LO_gc);

		/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
		USART_Format_Set(USART_data.usart, USART_CHSIZE_8BIT_gc,
						 USART_PMODE_DISABLED_gc, false);

		/* Enable RXC interrupt. */
		USART_RxdInterruptLevel_Set(USART_data.usart, USART_RXCINTLVL_LO_gc);

		/* Set Baudrate to 9600 bps:
		 * Use the default I/O clock frequency that is 2 MHz.
		 * Do not use the baudrate scale factor
		 *
		 * Baudrate select = (1/(16*(((I/O clock frequency)/Baudrate)-1)
		 *                 = 12
		 */
		USART_Baudrate_Set(&USART, 12 , 0);

		/* Enable both RX and TX. */
		USART_Rx_Enable(USART_data.usart);
		USART_Tx_Enable(USART_data.usart);

		/* Enable PMIC interrupt level low. */
		PMIC.CTRL |= PMIC_LOLVLEX_bm;
	}

	/*ISR(UART0_RECEIVE_INTERRUPT)
	{
		USART_RXComplete(&USART_data);
	}*/

	ISR(UART0_TRANSMIT_INTERRUPT)
	{
	    USART_DataRegEmpty(&USART_data);
	}

#else
#error "No definition for error_driver available"
#endif
#endif
