#include <stdint.h>
#include <stdbool.h>
#include "downlink.h"


const int DOWNLINK_PACKET_LENGTH = 4;

#ifdef SQUIRREL

void downlink_discover () {
	return;
}


/**
 * Handle RET package
 */
static bool downlink_handle_ret_package (struct downlink_packet *p) {

	// Utility functions
	switch (p->opcode & 0x0F) {
		case INFO_NUT:
		if (!find_nut_by_MAC(p)) {
			// Issue new running number
			// Insert into mac_lookup
			// Insert into class_lookup
		}
		case INFO_ID:
		// Determine running number
		// Insert into relevant lookup table

	}
}

void downlink_request_sensor_data(uint8_t class, uint8_t id) {


}



#endif

#ifdef NUT

/**
 * Handle GET package
 */
static inline bool downlink_handle_get_package (struct downlink_packet *p) {
	switch (p->opcode & 0x0F) {
		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] < GENERIC_ACTOR) {
				p->opcode = RET;
				p->value = get_value (p->id);
				return true;
			} else {
				return false;
			}
		case INFO_NUT:
			p->opcode = RET;
			p->id = 0;
			p->value = class_id_nut;
			return true;
		case INFO_EXTENSION:
			if (p->id < class_id_extensions_length) {
				p->opcode = RET;
				p->value = class_id_extensions[];
				return true;
			} else {
				return false;
			}
		case CONFIG:
			return false;
		default:
			return false;
	}
}

/**
 * Handle SET packages
 */
static inline bool downlink_handle_set_package (struct downlink_packet *p) {
	switch (p->opcode & 0x0F) {
		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] >= GENERIC_ACTOR) {
				p->opcode = RET;
				/*FIXME: Return Value?? */
				set_value (p->id, p->value);
				return true;
			} else {
				return false;
			}
		case CONFIG:
			return false;
		default:
			return false;
	}
}

/**
 * Major package downlink handling function
 */
bool downlink_handle_package (struct downlink_packet *p) {
	switch (p->opcode & 0xF0) {
		case GET:
			return downlink_handle_get_package (p);
		case SET:
			return downlink_handle_set_package (p);
		case ECHO:
			return true;
		default:
			return false;
	}
}
#endif

