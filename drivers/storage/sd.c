/*    ___                         ___           ___           ___          _____
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
      ___                       ___           ___           ___           ___           ___     
     /  /\          ___        /  /\         /  /\         /  /\         /  /\         /  /\    
    /  /:/_        /  /\      /  /::\       /  /::\       /  /::\       /  /:/_       /  /:/_   
   /  /:/ /\      /  /:/     /  /:/\:\     /  /:/\:\     /  /:/\:\     /  /:/ /\     /  /:/ /\  
  /  /:/ /::\    /  /:/     /  /:/  \:\   /  /:/~/:/    /  /:/~/::\   /  /:/_/::\   /  /:/ /:/_ 
 /__/:/ /:/\:\  /  /::\    /__/:/ \__\:\ /__/:/ /:/___ /__/:/ /:/\:\ /__/:/__\/\:\ /__/:/ /:/ /\
 \  \:\/:/~/:/ /__/:/\:\   \  \:\ /  /:/ \  \:\/:::::/ \  \:\/:/__\/ \  \:\ /~~/:/ \  \:\/:/ /:/
  \  \::/ /:/  \__\/  \:\   \  \:\  /:/   \  \::/~~~~   \  \::/       \  \:\  /:/   \  \::/ /:/ 
   \__\/ /:/        \  \:\   \  \:\/:/     \  \:\        \  \:\        \  \:\/:/     \  \:\/:/  
     /__/:/          \__\/    \  \::/       \  \:\        \  \:\        \  \::/       \  \::/   
     \__\/                     \__\/         \__\/         \__\/         \__\/         \__\/    

sd.c - SD lives here
*/

#include "sd.h"
//void sd_read_block(uint8_t *addr, uint8_t *read_buffer);
//void sd_write_block(uint8_t *addr, uint8_t *write_buffer);
void sd_send_buffer(void);
void sd_get_response(uint8_t response_type);

void sd_init() {
	spi_init();
	debug_pgm(PSTR("SD: SPI Init Succeeded"));
	// Place SD Card into Idle State
	sd_send_command(CMD0, NULL); 
	sd_get_response(R1);
	// Chip Select Low
	clear_bit(PORTD.OUT, 3);
	// Place SD Card into Idle State (again). Transition to SPI mode 
	sd_send_command(CMD0, NULL);
	sd_get_response(R1);
	// Check supply voltage
	sd_send_command(CMD8, NULL); 
	sd_get_response(R1);
	debug_pgm(PSTR("SD: Switch to SPI Mode Succeeded. Voltage queried"));
	if (sd_response_buffer[0] == 0x04) { // CMD8 is illegal, Version 1 card
		do {
			// Prepare 'A'-Command transmission
			sd_send_command(CMD55, NULL);
			sd_get_response(R1);
			// Card initialization. Doesn't know about High Capacity cards.
			sd_send_command(ACMD41, NULL);
			sd_get_response(R1);
		} while (sd_response_buffer[0] != 0x00);
		debug_pgm(PSTR("SD: Type 1 Initialization complete"));
	} else { // Version 2
		do {
			// Prepare 'A'-Command transmission
			sd_send_command(CMD55, NULL);
			sd_get_response(R1);
			// Card initialization. Inform it we don't like High Capacity cards.
			sd_send_command(ACMD41, NULL);
			sd_get_response(R1);
		} while (sd_response_buffer[0] != 0x00);
		// Gets the OCR [Operating Conditions Register] (including Card Capacity)
		sd_send_command(CMD58, NULL); 
		sd_get_response(R3);
		debug_pgm(PSTR("SD: Type 2 Initialization complete"));
	}
	// Set block size to 32 bytes
	//sd_send_command(CMD16, NULL);
	//sd_get_response(R1);
}

bool sd_send_command(uint8_t command_nr, uint8_t *arguments) {
	switch (command_nr) {
		case CMD0:
			// Static CMD0. As the card is still in SD mode a valid CRC is required
			//sd_buffer = 0x400000000095;
			sd_buffer[0] = 0x40;
			sd_buffer[1] = sd_buffer[2] = sd_buffer[3] = sd_buffer[4] = 0x00;
			sd_buffer[5] = 0x95;
			debug_pgm(PSTR("SD: CMD0 complete"));
			break;
		case CMD8:
			// Static CMD8. Ask the card if it likes tasty 2.7-3.6V
			//sd_buffer = 0x480000010001;
			sd_buffer[0] = 0x48;
			sd_buffer[1] = sd_buffer[2] = sd_buffer[4] = 0x00;
			sd_buffer[3] = sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: CMD8 complete"));
			break;
		case CMD16:
			// Static CMD16. Sets the block size to 32 bytes.
			//sd_buffer = 0x500000002001;
			sd_buffer[0] = 0x50;
			sd_buffer[1] = sd_buffer[2] = sd_buffer[3] = 0x00;
			sd_buffer[4] = 0x20;
			sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: CMD16 complete"));
			break;
		case CMD17:
			// Requests a single block beginning at the address passed (4 Bytes)
			sd_buffer[0] = 0x51;
			for (int i = 0; i < 4; i++)
				sd_buffer[i+1] = arguments[i];
			sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: CMD17 complete"));
			break;
		case CMD24:
			// Writes a single block at the address passed
			sd_buffer[0] = 0x58;
			for (int i = 0; i < 4; i++)
				sd_buffer[i+1] = arguments[i];
			sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: CMD24 complete"));
			break;
		case CMD55:
			// Static CMD55. Informs the card the next command will be an 'A'-Command (Application Specific)
			//sd_buffer = 0x770000000001;
			sd_buffer[0] = 0x77;
			sd_buffer[1] = sd_buffer[2] = sd_buffer[3] = sd_buffer[4] = 0x00;
			sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: CMD55 complete"));
			break;
		case ACMD41:
			// Static ACMD41. Initialize the card. Inform it we don't support High Capacity.
			//sd_buffer = 0x696000000001;
			sd_buffer[0] = 0x69;
			sd_buffer[1] = 0x60;
			sd_buffer[2] = sd_buffer[3] = sd_buffer[4] = 0x00;
			sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: ACMD41 complete"));
			break;
		case CMD58:
			// Static CMD58. Get OCR. (Operating Conditions Register) Returns the Card Capacity. 
			//sd_buffer = 0x790000000001;
			sd_buffer[0] = 0x79;
			sd_buffer[1] = sd_buffer[2] = sd_buffer[3] = sd_buffer[4] = 0x00;
			sd_buffer[5] = 0x01;
			debug_pgm(PSTR("SD: CMD58 complete"));
			break;
		default:
			return false;
	}
	sd_send_buffer();
	return true;
}

void sd_read_bytes(uint32_t addr, uint8_t *read_buffer, uint16_t size) {
	uint32_t block_addr = addr - (addr % SD_BLOCK_SIZE);
	uint8_t bytes_read = 0;
	while (bytes_read < size) {
	uint8_t addr_bytes[4];
	 
	addr_bytes[0] = block_addr;
	addr_bytes[1] = block_addr>>8;
	addr_bytes[2] = block_addr>>16;
	addr_bytes[3] = block_addr>>24;
	if(sd_send_command(CMD17, addr_bytes))
	{
		debug_pgm(PSTR("SD: CMD17 Succeeded"));
		sd_get_response(R1);
		if (sd_response_buffer[0] == 0x00)
		{
			for (int i = 0; i < 3+SD_BLOCK_SIZE; i++)
			{
				sd_token_buffer[i] = spi_receive_byte();
				if ((i == 0) && (sd_token_buffer[i] != 0xFE))
				{ // If the first token is not a valid token bail out.
					return;
				}
				if (i-1+block_addr >= addr && bytes_read < size) 
					{
					if (read_buffer != NULL) 
					read_buffer[bytes_read++] = sd_token_buffer[i];
					}
			}
		}
	} else { debug_pgm(PSTR("SD: Read Failed")); return;}
	block_addr += SD_BLOCK_SIZE;
	}
	/*for (int i = 0; i < size; i++) {
		read_buffer[i] = sd_token_buffer[i+2];
	}*/
	debug_pgm(PSTR("SD: Read Succeeded"));
}


void sd_write_bytes(uint32_t addr, uint8_t *write_buffer, uint16_t size) {
	uint32_t block_addr = addr - (addr % SD_BLOCK_SIZE);
	uint8_t bytes_written = 0;
	while (bytes_written < size) {
	sd_read_bytes(block_addr, NULL, SD_BLOCK_SIZE);
	uint8_t addr_bytes[4];
	addr_bytes[0] = block_addr;
	addr_bytes[1] = block_addr>>8;
	addr_bytes[2] = block_addr>>16;
	addr_bytes[3] = block_addr>>24;
	if (sd_send_command(CMD24, addr_bytes)) {
		debug_pgm(PSTR("SD: CMD24 Succeeded"));
		sd_get_response(R1);
		if (sd_response_buffer[0] == 0x00) {
			uint8_t start_token = 0xFE;
			spi_send_byte(start_token);
			for (int i = 0; i < 512; i++) {
			if (block_addr + i < addr || block_addr + i > addr+size)
				spi_send_byte(sd_token_buffer[i+1]);
			else {
				spi_send_byte(write_buffer[bytes_written++]);
			}
			uint8_t crc[2] = {0xFF, 0xFF}; //0xFFFF;
			spi_send_byte(crc[0]);
			spi_send_byte(crc[1]);
		}
		sd_token_buffer[0] = spi_receive_byte(); // Receive data response token. TODO: Check status bits
		// Busy tokens?
			
		}
		block_addr += SD_BLOCK_SIZE;
	}
	debug_pgm(PSTR("SD: Write Succeeded"));
	}
}

void sd_send_buffer() {
	for (int i = 0; i < 6; i++) {
		spi_send_byte(sd_buffer[i]);
		debug_pgm(PSTR("SD: Buffer Byte Sent"));
	}
}

void sd_get_response(uint8_t response_type) {
	switch (response_type) {
		case R1:
			sd_response_buffer[0] = spi_receive_byte();
			debug_pgm(PSTR("SD: R1 received"));
			break;
		case R1b:
			sd_response_buffer[0] = spi_receive_byte();
			do {
				sd_response_buffer[1] = spi_receive_byte();
			} while (sd_response_buffer[1] == 0x00);
			debug_pgm(PSTR("SD: R1b received"));
			break;
		case R2:
			sd_response_buffer[0] = spi_receive_byte();
			sd_response_buffer[1] = spi_receive_byte();
			debug_pgm(PSTR("SD: R2 received"));
			break;
		case R3:
		case R7:
			sd_response_buffer[0] = spi_receive_byte();
			sd_response_buffer[1] = spi_receive_byte();
			sd_response_buffer[2] = spi_receive_byte();
			sd_response_buffer[3] = spi_receive_byte();
			sd_response_buffer[4] = spi_receive_byte();
			debug_pgm(PSTR("SD: R3/7 received"));
			break;
		default:
			return;
	}
}

