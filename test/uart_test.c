#include <avr/io.h>
#include <util/delay.h>
#include "../drivers/uart/uart.h"
#include <avr/interrupt.h>

#include "../drivers/uart/usart_driver.h"

#define UART_BAUD_RATE      9600ul

/*! Define that selects the Usart used in example. */
#define USART USARTC0

int main_old(void)
{

	//Change clock to 32Mhz
	/*OSC.CTRL = OSC_RC32MEN_bm; //enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));   //wait for stability
	CCP = CCP_IOREG_gc; //secured access
	CLK.CTRL = 0x01; //choose this osc source as clk*/



	PORTC.DIR |= (1<<PIN5);

		_delay_ms(500);
		PORTC.OUT |= (1<<PIN5);
		_delay_ms(5000);
		PORTC.OUT &= ~(1<<PIN5);
		_delay_ms(500);


		/* This PORT setting is only valid to USARTC0 if other USARTs is used a
		 * different PORT and/or pins is used. */
		/* PIN3 (TXD0) as output. */
		PORTC.DIRSET = PIN3_bm;

		/* PC2 (RXD0) as input. */
		PORTC.DIRCLR = PIN2_bm;

		/* USARTC0, 8 Data bits, No Parity, 1 Stop bit. */
		USART.CTRLC = (uint8_t) USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc;



		/* Set Baudrate to 9600 bps:
		 * Use the default I/O clock fequency that is 2 MHz.
		 * Do not use the baudrate scale factor
		 *
		 * Baudrate select = (1/(16*(((I/O clock frequency)/Baudrate)-1)
		 *                 = 12
		 */
		USART.BAUDCTRLA =(uint8_t)UART_BAUD_SELECT(9600,F_CPU);
		USART.BAUDCTRLB =(UART_BAUD_SELECT(9600,F_CPU) >> 8);

		/* Enable both RX and TX. */

		USART.CTRLB |= USART_RXEN_bm;


		/*! \brief Disable USART receiver.
		 *
		 *  \param _usart Pointer to the USART module.
		 */
		USART.CTRLB |= USART_TXEN_bm;

		while(1) {

			uint8_t i = 0;
			for (i=0; i<255; i++)
			{
			    /* Send one char. */
				do{
				/* Wait until it is possible to put data into TX data register.
				 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
				}while(!((USART.STATUS & USART_DREIF_bm) != 0));
				USART.DATA = i;
				   PORTC.OUT |= (1<<PIN5);
				   _delay_ms(50);
				   PORTC.OUT &= ~(1<<PIN5);
				   _delay_ms(100);
			}
			 _delay_ms(1000);
		}
		return 0;
}

int main(void)
{

	PORTB.DIR |= (1<<PIN4);
	PORTB.DIR |= (1<<PIN6);
while(1){
		PORTB.OUT |= (1<<PIN4);
		_delay_ms(500);
		PORTB.OUT &= ~(1<<PIN4);
		PORTB.OUT |= (1<<PIN6);
		_delay_ms(500);
		PORTB.OUT &= ~(1<<PIN6);
}



			_delay_ms(500);
	//}

	//return 0;

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
	sei();

	/*while(1) {
		uint8_t i = 0;
		for (i=0; i<255; i++)
		{
			// Send one char.
			do{
			* Wait until it is possible to put data into TX data register.
			 * NOTE: If TXDataRegister never becomes empty this will be a DEADLOCK. */
			/*}while(!((USART.STATUS & USART_DREIF_bm) != 0));
			uart_putc(i);
			   PORTC.OUT |= (1<<PIN5);
			   _delay_ms(50);
			   PORTC.OUT &= ~(1<<PIN5);
			   _delay_ms(100);
		}
		 _delay_ms(1000);
	}*/
	 while(1)
	 {
		 uint16_t byte =  uart_getc();
		 if (byte != UART_NO_DATA)
		 {
			 uart_putc(byte);
			 PORTC.OUT |= (1<<PIN5);
						   _delay_ms(50);
						   PORTC.OUT &= ~(1<<PIN5);
						   _delay_ms(100);
		 }
	 }
}
