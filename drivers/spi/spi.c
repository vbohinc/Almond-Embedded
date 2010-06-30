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

void spi_init() {
	// Assert low on SS - SPI connected to PORTD
	//clear_bit(PORTD_OUT, 4);
	set_bit(PORTD.OUT, 4); // Sets Slave Select (SS) as Output
	set_bit(SPID_CTRL, 6); // Enable SPI
	set_bit(SPID_CTRL, 4); // Set to Master
}
void spi_send_byte(uint8_t byte_to_send) {
	SPID_DATA = byte_to_send;
	while (!check_bit(SPID_STATUS,7)); // Wait until IF is set to signal end of Tx
	debug_pgm(PSTR("SPI: Byte Sent"));
	return;
}
uint8_t spi_receive_byte() {
	while (!check_bit(SPID_STATUS,7)); // Wait until IF is set to signal end of Rx
	debug_pgm(PSTR("SPI: Byte Received"));
	return SPID_DATA;
}

