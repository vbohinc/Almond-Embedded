#include "spi.h"
#include <util/delay.h>

#define SPIUARTPORT USARTD0

#define SCK  7  // USART XCK
#define MISO 6  // USART RxD
#define MOSI 5  // USART TxD
#define CS   0  // just an output pin

#define SPIPORT PORTC
#define SPIMOD  SPIC

uint8_t sd_raw_xmit_byte(uint8_t b)
{
	/* Send pattern. */
	SPIMOD.DATA = b;

	/* Wait for transmission complete. */
	debug_pgm(PSTR("Wait for transmission complete"));
	while(!(SPIMOD.STATUS & SPI_IF_bm)) {

	}
debug_pgm(PSTR("transmission complete"));
	/* Read received data. */
	uint8_t result = SPIMOD.DATA;
	byte_to_hex(result);

	return(result);
}

void spi_init() {
	debug_pgm(PSTR("spi_init"));

	SPIMOD.CTRL   = SPI_PRESCALER_DIV4_gc |                  /* SPI prescaler. */
	                      (false ? SPI_CLK2X_bm : 0) |     /* SPI Clock double. */
	                      SPI_ENABLE_bm |                  /* Enable SPI module. */
	                      (false ? SPI_DORD_bm  : 0) |  /* Data order. */
	                      SPI_MASTER_bm |                  /* SPI master. */
	                      SPI_MODE_0_gc;                   /* SPI mode. */

	/* Interrupt level. */
	SPIMOD.INTCTRL = SPI_INTLVL_OFF_gc;

	set_bit(SPIPORT.DIR, MOSI);
	set_bit(SPIPORT.DIR, CS);
	clear_bit(SPIPORT.DIR, MISO);
	set_bit(SPIPORT.DIR, SCK);

	set_bit(SPIPORT.OUT, CS);


	



	/*// Assert low on SS - SPI connected to SPIPORT
	//clear_bit(SPIPORT_OUT, 4);
	set_bit(SPIPORT.OUT, 4); // Sets Slave Select (SS) as Output
	set_bit(SPIPORT.OUT, 5); // Set MOSI and SCK as Output
	set_bit(SPIPORT.OUT, 7);
	set_bit(SPID.CTRL, 6); // Enable SPI
	set_bit(SPID.CTRL, 4); // Set to Master
	set_bit(SPID.CTRL, 7); // CLK2X
	//clear_bit(SPID.CTRL, 0); // Prescaler to 00 (by default)
	//clear_bit(SPID.CTRL, 1); // Results in SCK frequency = clk_PER/2*/

   /*// CS und MOSI High
	set_bit(SPIPORT.OUT, CS); // nicht aktiv
	set_bit(SPIPORT.OUT, MOSI);
	// CLK Low
	clear_bit(SPIPORT.OUT, SCK);

	// CS, CLK und MOSI als Ausgänge
	set_bit(SPIPORT.DIRSET, MOSI);
	set_bit(SPIPORT.DIRSET, SCK);
	set_bit(SPIPORT.DIRSET, CS);

	// MISO als Eingang
	set_bit(SPIPORT.DIRCLR, MISO);

	// 100Khz SPI Frequenz zur Initialisierung
	// BSEL = 32e6 / (2 * Baudrate) - 1
	// BSEL = 2e6 / (2 * Baudrate) - 1
	SPIUARTPORT.BAUDCTRLA = 159;
	SPIUARTPORT.BAUDCTRLB = 0;

	// Frameformat (MSB first), SPI Mode 0
	SPIUARTPORT.CTRLC = USART_CMODE_MSPI_gc;

	SPIUARTPORT.CTRLA = 0;

	// enable RX und TX
	SPIUARTPORT.CTRLB = USART_RXEN_bm | USART_TXEN_bm;*/


}

void spi_send_byte(uint8_t byte_to_send) {
	/*SPID.DATA = byte_to_send;
	while (!check_bit(SPID.STATUS,7)); // Wait until IF is set to signal end of Tx*/
	sd_raw_xmit_byte(byte_to_send);
	//debug_pgm(PSTR("SPI: Byte Sent"));
	/*return;*/
}

uint8_t spi_receive_byte() {

	/*SPID.DATA = 0xFF;
	_delay_ms(10);

	while (!check_bit(SPID.STATUS,7)); // Wait until IF is set to signal end of Rx*/
	//debug_pgm(PSTR("SPI: Byte Receiving"));
	return sd_raw_xmit_byte(0xFF);
	/*return SPID.DATA;*/
}

