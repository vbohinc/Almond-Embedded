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
	// Assert low on SS - SPI connected to PORTD
	//clear_bit(PORTD_OUT, 4);
	set_bit(PORTD.DIR, 3); // Sets Slave Select (SS) (on atxmega) as Output

	set_bit(PORTD.DIR, 5); // Set MOSI and SCK as Output and MISO as Output
	clear_bit(PORTD.DIR, 6);
	set_bit(PORTD.DIR, 7);

	set_bit(SPID.CTRL, 6); // Enable SPI
	set_bit(SPID.CTRL, 4); // Set to Master
	//set_bit(SPID.CTRL, 7); // CLK2X
	set_bit(SPID.CTRL, 0); // Prescaler to 00 (by default)
	set_bit(SPID.CTRL, 1); // Results in SCK frequency = clk_PER/2
}

void spi_send_byte(uint8_t byte_to_send) {
	SPID.DATA = byte_to_send;
	while (!check_bit(SPID.STATUS,7));
        //{
            //debug_pgm(PSTR("SPI: Waiting"));
        //    _delay_ms(100);
        //    error_putc(SPID.STATUS);
        //}// Wait until IF is set to signal end of Tx
	//debug_pgm(PSTR("SPI: Byte Sent"));
	return;
}
uint8_t spi_receive_byte() {

	//SPID.DATA = 0xFF;
	//_delay_ms(10);

	while (!check_bit(SPID.STATUS,7)); // Wait until IF is set to signal end of Rx
	//debug_pgm(PSTR("SPI: Byte Received"));
	return SPID.DATA;
}

