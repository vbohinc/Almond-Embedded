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

void spi_init() {

	/* set port directions, and initial values*/
	// Assert low on SS - SPI connected to PORTD
	set_bit(PORTD.DIR, 4);
	set_bit(PORTD.DIR, 3); // Sets Slave Select (SS) (on atxmega) as Output
	set_bit(PORTD.OUT, 4);
	set_bit(PORTD.OUT, 3);

	set_bit(PORTD.DIR, 5); // Set MOSI and SCK as Output and MISO as Output
	clear_bit(PORTD.DIR, 6);
	set_bit(PORTD.DIR, 7);

	for( int a=0; a<2000; a++); // waits a short time

	/* init spi*/
	set_bit(SPID.CTRL, 6); // Enable SPI
	set_bit(SPID.CTRL, 4); // Set to Master
	clear_bit(SPID.CTRL, 7); // CLK2X
	set_bit(SPID.CTRL, 1); // Prescaler to 11
	set_bit(SPID.CTRL, 0); // Results in SCK frequency = clk_PER/128
}

void spi_send_byte(uint8_t byte_to_send) {
	clear_bit(PORTD.OUT, 3); // Pulls Slave Select (SS) low
	SPID.DATA = byte_to_send;
	while (!check_bit(SPID.STATUS,7));
        //{
            //debug_pgm(PSTR("SPI: Waiting"));
        //    _delay_ms(100);
        //    error_putc(SPID.STATUS);
        //}// Wait until IF is set to signal end of Tx
	//debug_pgm(PSTR("SPI: Byte Sent"));
	set_bit(PORTD.OUT, 3); // Sets Slave Select (SS) high
	return;
}
uint8_t spi_receive_byte() {
	clear_bit(PORTD.OUT, 3); // Pulls Slave Select (SS) low
	//SPID.DATA = 0xFF;
	_delay_ms(10);
	//for( int a=0; a<2000; a++); // waits a short time
	while (!check_bit(SPID.STATUS,7)); // Wait until IF is set to signal end of Rx
	debug_pgm(PSTR("SPI: Byte Received"));
	set_bit(PORTD.OUT, 3); // Sets Slave Select (SS) high
	return SPID.DATA;
}

