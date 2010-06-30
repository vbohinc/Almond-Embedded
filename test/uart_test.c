#include "uart.h"

#define UART_BAUD_RATE      19200

int main()
{
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

	uart_putc(1);
}
