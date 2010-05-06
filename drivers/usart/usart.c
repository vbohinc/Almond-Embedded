/**
* @file usart.c
* @author Stefan Profanter
* @date 26.04.2010
*
* This file contanis the bodys for the functions available to
* communicate with the USART Module and configure it on the AVR microcontroller.
* Due to different used microcontrollers and therefore different USART registers
* two defines are used to distinguish between the two microcontrollers:
* ATmega8535   = ATMEGA8535
* ATxmega128A1 = ATXMEGA
*
*/

#include "usart.h"

#ifdef ATMEGA8535


#elif ATXMEGA
#	include "usart_driver.h"
	/*! Input data buffer for interrupt on receive */
	USART_data_t USART_data;

	/*! Define that selects the Usart used in example. */
	#define USART USARTC0
#elif SERIAL

#else
#	error ATMEGA8535 or ATXMEGA not defined
#endif

/**
* @TODO maxi fragen, wo btm-222 beim atxmega dranh�ngt
*/


void usart_set_byte_handler(void (*byte_handling_function)(const uint8_t))
{
	usart_byte_handling_function = byte_handling_function;
}

void usart_init(void)
{

#ifdef ATMEGA8535
	/* Set baud rate */
	UBRRH = (unsigned char)(USART_UBRR_BAUD>>8);
	UBRRL = (unsigned char)USART_UBRR_BAUD;

	/* Set frame format: 8data bit and default 1 stopbit */
	UCSRC = (3<<UCSZ0);

	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);



#elif ATXMEGA

	/* This PORT setting is only valid to USARTC0 if other USARTs is used a
	 * different PORT and/or pins is used. */
	/* PIN3 (TXD0) as output. */
	PORTC.DIRSET = PIN3_bm;

	/* PC2 (RXD0) as input. */
	PORTC.DIRCLR = PIN2_bm;


	/* Use USARTC0 and initialize buffers. */
	USART_InterruptDriver_Initialize(&USART_data, &USART, USART_DREINTLVL_LO_gc);

	/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
	USART_Format_Set(&USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);

	/* Enable RXC interrupt. */
	USART_RxdInterruptLevel_Set(USART_data.usart, USART_RXCINTLVL_LO_gc);

	/* Set Baudrate
	 * Do not use the baudrate scale factor
	 *
	 */
	USART_Baudrate_Set(&USART, USART_UBRR_BAUD , 0);

	/* Enable both RX and TX. */
	USART_Rx_Enable(&USART);
	USART_Tx_Enable(&USART);

	/* Enable PMIC interrupt level low. */
	PMIC.CTRL |= PMIC_LOLVLEX_bm;

	/* Enable global interrupts. */
	sei();
#endif
}

uint8_t usart_putc (const uint8_t byte, const uint8_t max_try_count)
{

#ifdef ATMEGA8535
    // Wait until UDR is ready for new value
	if (max_try_count>0)
	{
    	uint8_t count;
		for (count=0; count<max_try_count && (!(UCSRA & (1 << UDRE))); count++)
		{
			_delay_ms(10);
		}
		if (count==max_try_count)
			return 0;
	}

    // Writing UDR starts sending
    UDR = byte;
#elif ATXMEGA
	// Wait until write buffer is ready for new value

    if (max_try_count > 0)
    {
    	uint8_t count;
		for (count=0; count<max_try_count && (!USART_IsTXDataRegisterEmpty(&USART)); count++)
		{
			_delay_ms(10);
		}
		if (count==max_try_count)
			return 0;
    }

	USART_PutChar(&USART, byte);
#endif

    return 1;
}

uint8_t usart_send_bytes(uint8_t *bytes, const uint8_t length , const uint8_t max_try_count)
{
	for (uint8_t i=0; i<length; i++)
	{
		if (usart_putc(bytes[i], max_try_count)==0)
			return 0;
	}

	return 1;
}

#ifdef ATMEGA8535
// Empfangene Zeichen werden in die Eingabgs-FIFO gespeichert und warten dort 
ISR(USART_RX_vect)
{
	usart_byte_received(UDR);

}

#elif ATXMEGA

/*! \brief Receive complete interrupt service routine.
 *
 *  Receive complete interrupt service routine.
 *  Calls the common receive complete handler with pointer to the correct USART
 *  as argument.
 */
ISR(USARTC0_RXC_vect)
{
	USART_RXComplete(&USART_data);

	if (USART_RXBufferData_Available(&USART_data))
	{
		usart_byte_received(USART_RXBuffer_GetByte(&USART_data));
	}

}


/*! \brief Data register empty  interrupt service routine.
 *
 *  Data register empty  interrupt service routine.
 *  Calls the common data register empty complete handler with pointer to the
 *  correct USART as argument.
 */
ISR(USARTC0_DRE_vect)
{
	USART_DataRegEmpty(&USART_data);
}
#endif


void usart_byte_received(const uint8_t byte)
{


	usart_byte_handling_function(byte);

}




