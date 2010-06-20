/* static uint8_t[6] sd_buffer;
static uint8_t[5] sd_response_buffer;
static uint8_t[35] sd_token_buffer; */

void sd_init() {
	spi_init();
	
	sd_send_command(CMD0, NULL); // Place SD Card into Idle State
	clear_bit(SPID_CTRL, 3); // Chip Select Low
	sd_send_command(CMD0, NULL); // Place SD Card into Idle State (again). Transition to SPI mode
	sd_send_command(CMD8, NULL); // Check supply voltage
	sd_get_response(R1);
	if (sd_response_buffer[0] == 0x04) { // CMD8 is illegal, Version 1 card
		sd_send_command(CMD55, NULL); // Prepare 'A'-Command transmission
		sd_send_command(ACMD41, NULL); // Card initialization. Doesn't know about High Capacity cards.
		do {sd_get_response(R1);} while (!(sd_response_buffer[0] == 0x00));
		return;
		
	} else { // Version 2
		sd_send_command(CMD55, NULL); // Prepare 'A'-Command transmission
		sd_send_command(ACMD41, NULL); // Card initialization. Inform it we don't like High Capacity cards.
		sd_send_command(CMD58, NULL); // Gets the OCR [Operating Conditions Register] (including Card Capacity)
		do {sd_get_response(R1);} while (!(sd_response_buffer[0] == 0x00));
		return;
	}
}
boolean sd_send_command(uint8_t command_nr, uint8_t *arguments) {
	switch (command_nr) {
		case: CMD0
		sd_buffer = 0x400000000095; // Static CMD0. As the card is still in SD mode a valid CRC is required
		break;
		case: CMD8
		sd_buffer = 0x480000010001; // Static CMD8. Ask the card if it likes tasty 2.7-3.6V
		break;
		case: CMD55
		sd_buffer = 0x770000000001; // Static CMD55. Informs the card the next command will be an 'A'-Command (Application Specific)
		break;
		case: ACMD41
		sd_buffer = 0x696000000001; // Static ACMD41. Initialize the card. Inform it we don't support High Capacity.
		break;
		case: CMD58
		sd_buffer = 0x790000000001; // Static CMD58. Get OCR. Returns the Card Capacity.
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
				
			}
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
		do {spi_receive_byte(sd_response_buffer[1])} while (sd_response_buffer[1] == 0x00);
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
