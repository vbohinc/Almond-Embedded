#include <stdint.h>
#include <stdbool.h>

#include "downlink.h"

static bool check_checksum (struct downlink_packet *packet) {

	// Calculate CRC 16
	
	return true;
}


bool downlink_handle_packet (struct downlink_packet *packet) {
	
	if (!check_checksum (packet)) return false;

	/*
	packet->TID = 5;
	packet->opcode = "
	*/

	return true;
}



