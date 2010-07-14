#include "../drivers/storage/sd.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#define NUM_BYTES 254

uint8_t bytes[NUM_BYTES];

void print_hex(uint8_t num)
{
	if (num<10)
		error_putc(num+48);
	else
	{
		switch (num)
		{
		case 10:
			error_putc('A'); break;
		case 11:
			error_putc('B'); break;
		case 12:
			error_putc('C'); break;
		case 13:
			error_putc('D'); break;
		case 14:
			error_putc('E'); break;
		case 15:
			error_putc('F'); break;
		default:
			error_putc('#'); break;
		}
	}
}

void byte_to_hex(uint8_t byte){
	uint8_t b2 = (byte & 0x0F);
	uint8_t b1 = ((byte & 0xF0)>>4);
	print_hex(b1);
	print_hex(b2);
}

uint8_t main(void) {

	error_init();

	sei();
	debug_pgm(PSTR("### Storage Test ###"));
	/*spi_init();
	for (uint8_t i = 0; i < 256; i++) {
	spi_send_byte(0xAA);
	_delay_ms(10);
	//spi_receive_byte();
	}
	_delay_ms(1000);
	while (1) spi_send_byte(0x00);*/
	sd_init();
	/*uint8_t b = 0;
	for (uint16_t i=0; i<NUM_BYTES; i++)
	{
		bytes[i] = b;
		b++;
	}

	//sd_write_bytes(0, bytes, NUM_BYTES);

	sd_read_bytes(0,bytes,50);

	debug_pgm(PSTR("Read bytes:"));

	for (uint8_t i=0; i<50; i++)
	{
		byte_to_hex(bytes[i]);
		error_putc(' ');
		if ((i+1)%8==0)
			error_putc(' ');
		if ((i+1)%16==0)
			error_putc('\n');


	}*/
	return 0;
}
