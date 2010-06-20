
void sd_init() {
	spi_init();
	// Place SD Card into Idle State
	sd_send_command(CMD0, NULL); 
	sd_get_response(R1);
	// Chip Select Low
	clear_bit(SPID_CTRL, 3);
	// Place SD Card into Idle State (again). Transition to SPI mode 
	sd_send_command(CMD0, NULL);
	sd_get_response(R1);
	// Check supply voltage
	sd_send_command(CMD8, NULL); 
	sd_get_response(R1);
	if (sd_response_buffer[0] == 0x04) { // CMD8 is illegal, Version 1 card
		do {
		// Prepare 'A'-Command transmission
		sd_send_command(CMD55, NULL); 
		sd_get_response(R1);
		// Card initialization. Doesn't know about High Capacity cards.
		sd_send_command(ACMD41, NULL); 
		sd_get_response(R1);
		} while (!(sd_response_buffer[0] == 0x00));
		return;
		
	} else { // Version 2
		do {
		// Prepare 'A'-Command transmission
		sd_send_command(CMD55, NULL); 
		sd_get_response(R1);
		// Card initialization. Inform it we don't like High Capacity cards.
		sd_send_command(ACMD41, NULL); 
		sd_get_response(R1);
		} while (!(sd_response_buffer[0] == 0x00));
		// Gets the OCR [Operating Conditions Register] (including Card Capacity)
		sd_send_command(CMD58, NULL); 
		sd_get_response(R1);
		return;
	}
	// Set block size to 32 bytes
	sd_send_command(CMD16, NULL); 
	sd_get_response(R1);
}
boolean sd_send_command(uint8_t command_nr, uint8_t *arguments) {
	switch (command_nr) {
		case: CMD0
		// Static CMD0. As the card is still in SD mode a valid CRC is required
		sd_buffer = 0x400000000095; 
		break;
		case: CMD8
		 // Static CMD8. Ask the card if it likes tasty 2.7-3.6V
		sd_buffer = 0x480000010001;
		break;
		case: CMD16
		// Static CMD16. Sets the block size to 32 bytes.
		sd_buffer = 0x500000002001; 
		break;
		case: CMD17 
		// Requests a single block beginning at the address passed (4 Bytes)
		sd_buffer[0] = 0x51;
		for (int i = 0; i < 4; i++) sd_buffer[i+1] = arguments[i];
		sd_buffer[5] = 0x01; 
		break;
		case: CMD55
		// Static CMD55. Informs the card the next command will be an 'A'-Command (Application Specific)
		sd_buffer = 0x770000000001; 
		break;
		case: ACMD41
		// Static ACMD41. Initialize the card. Inform it we don't support High Capacity.
		sd_buffer = 0x696000000001; 
		break;
		case: CMD58
		// Static CMD58. Get OCR. Returns the Card Capacity.
		sd_buffer = 0x790000000001; 
		break;
		default:
		return false;
	}
	sd_send_buffer();
	return true;
}
void sd_read_block(uint8_t *addr, uint8_t *read_buffer) {
	if(sd_send_command(CMD17, addr)) {
		sd_get_response(R1);
		if (sd_response_buffer[0] == 0x00) {
			for (int i = 0; i < 35; i++) {
				spi_receive_byte(sd_token_buffer[i]);
				if ((i == 0) && (sd_token_buffer[i] != 0xFE)) {
					return;
				}
			}
		}
	}
	for (int i = 0; i < 32; i++) {
		read_buffer[i] = sd_token_buffer[i+2];
	}
}


void sd_write_block(uint8_t *addr, uint8_t *write_buffer) {
	
}


void sd_send_buffer() {
	for (int i = 0; i < 6; i++) {
		spi_send_byte(sd_buffer[i]);
	}
}
void sd_get_response(uint8_t response_type) {
	switch (response_type) {
		case: R1
		spi_receive_byte(sd_response_buffer[0]);
		break;
		case: R1b
		spi_receive_byte(sd_response_buffer[0]);
		do {spi_receive_byte(*sd_response_buffer[1])} while (sd_response_buffer[1] == 0x00);
		break;
		case: R2
		spi_receive_byte(sd_response_buffer[0]);
		spi_receive_byte(sd_response_buffer[1]);
		break;
		case: R3, R7
		spi_receive_byte(sd_response_buffer[0]);
		spi_receive_byte(sd_response_buffer[1]);
		spi_receive_byte(sd_response_buffer[2]);
		spi_receive_byte(sd_response_buffer[3]);
		spi_receive_byte(sd_response_buffer[4]);
		break;
		default:
		return;
	}
}
