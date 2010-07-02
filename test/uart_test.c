#include <avr/io.h>
#include <avr/delay.h>
#include "../drivers/uart/uart.h"
#include <avr/interrupt.h>

#define UART_BAUD_RATE      19200

int main()
{
sei();
PORTC.DIR |= (1<<PIN5);
PORTC.DIR |= (1<<PIN7);
//PORTC.OUT |= (1<<PIN7);
//while(1) {}


//while (1) {
PORTC.OUT |= (1<<PIN5);
PORTC.OUT |= (1<<PIN7);
_delay_ms(1000);
PORTC.OUT &= ~(1<<PIN5);
PORTC.OUT &= ~(1<<PIN7);
_delay_ms(1000);
//}

//return 0;

	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

	while(1) {
_delay_ms(3000);

PORTC.OUT |= (1<<PIN5);

	
	
	uart_putc((unsigned)0);
	for( int8_t i=0; i<255; i--) {
		_delay_ms(500);	
		uart_putc(i);
	}

	/*for( int i=0; i<8; i++) {
		_delay_ms(500);	
		uart_putc((unsigned)((unsigned)1)<<i);
	}*/
PORTC.OUT &= ~(1<<PIN5);
_delay_ms(100);
PORTC.OUT |= (1<<PIN5);
	/*for( int i=0; i<8; i++) {
		_delay_ms(500);
		uart_putc(~(1<<i));
	}*/

PORTC.OUT &= ~(1<<PIN5);
}
}
