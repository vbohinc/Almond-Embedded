/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
      ___           ___               
     /  /\         /  /\      ___     
    /  /:/_       /  /::\    /  /\    
   /  /:/ /\     /  /:/\:\  /  /:/    
  /  /:/ /::\   /  /:/~/:/ /__/::\    
 /__/:/ /:/\:\ /__/:/ /:/  \__\/\:\__ 
 \  \:\/:/~/:/ \  \:\/:/      \  \:\/\
  \  \::/ /:/   \  \::/        \__\::/
   \__\/ /:/     \  \:\        /__/:/ 
     /__/:/       \  \:\       \__\/  
     \__\/         \__\/              
*/
#include "spi.h"
#include <util/delay.h>

#define SCK  5  // USART XCK
#define MISO 6  // USART RxD
#define MOSI 7  // USART TxD
#define CS   7  // just an output pin

uint8_t sd_raw_xmit_byte(uint8_t b)
{
   while (! (USARTE1.STATUS & USART_DREIF_bm)); //debug_pgm(PSTR("Warten bis Datenregister leer")); // Warten bis Datenregister leer
   USARTE1.DATA = b; //debug_pgm(PSTR("Byte in Datenregister schreiben (Setup: steigende Flanken)")); // Byte in Datenregister schreiben (Setup: steigende Flanken)
   while (! (USARTE1.STATUS & USART_RXCIF_bm)); //debug_pgm(PSTR("Warten bis Antwortbyte empfangen (Sample: fallende Flanken)")); // Warten bis Antwortbyte empfangen (Sample: fallende Flanken)
   return USARTE1.DATA; //debug_pgm(PSTR("Antwortbyte")); // Antwortbyte
}

void spi_init() {
debug_pgm(PSTR("spi_init"));
	/*// Assert low on SS - SPI connected to PORTD
	//clear_bit(PORTD_OUT, 4);
	set_bit(PORTD.OUT, 4); // Sets Slave Select (SS) as Output
	set_bit(PORTD.OUT, 5); // Set MOSI and SCK as Output
	set_bit(PORTD.OUT, 7);
	set_bit(SPID.CTRL, 6); // Enable SPI
	set_bit(SPID.CTRL, 4); // Set to Master
	set_bit(SPID.CTRL, 7); // CLK2X
	//clear_bit(SPID.CTRL, 0); // Prescaler to 00 (by default)
	//clear_bit(SPID.CTRL, 1); // Results in SCK frequency = clk_PER/2*/

   // CS und MOSI High
	set_bit(PORTD.OUT, CS); // nicht aktiv
	set_bit(PORTE.OUT, MOSI);
	// CLK Low
	clear_bit(PORTE.OUT, SCK);

	// CS, CLK und MOSI als Ausgänge
	set_bit(PORTE.DIRSET, MOSI);
	set_bit(PORTE.DIRSET, SCK);
	set_bit(PORTD.DIRSET, CS);

	// MISO als Eingang
	set_bit(PORTE.DIRCLR, MISO);

	// 100Khz SPI Frequenz zur Initialisierung
	// BSEL = 32e6 / (2 * Baudrate) - 1
	// BSEL = 2e6 / (2 * Baudrate) - 1
	USARTE1.BAUDCTRLA = 159;
	USARTE1.BAUDCTRLB = 0;

	// Frameformat (MSB first), SPI Mode 0
	USARTE1.CTRLC = USART_CMODE_MSPI_gc;

	USARTE1.CTRLA = 0;

	// enable RX und TX
	USARTE1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;


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

