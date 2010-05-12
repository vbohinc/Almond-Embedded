#include <stdint.h>
#include <stdbool.h>
#include "downlink.h"
#include "../classes.h"

#ifdef SQUIRREL

void downlink_discover () {
	return;
}
// Handles a RET package received
static bool ret_package (struct downlink_packet *p) {


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

#endif

#ifdef NUT
static inline bool downlink_handle_get_package (struct downlink_packet *p) {

	switch (p->opcode & 0x0F) {
		case STANDARD: // Clarify numbers etc...? Maybe adjust proto
			if (p->id < 128 && class_id_sensors[p->id] != INVALID) {
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
		case INFO_ID:
			p->opcode = RET;			
			p->id = 0;
			p->value = class_id_nut;
			return true;
		default:
			return false;
	}
}

static inline bool downlink_handle_set_package (struct downlink_packet *p) {
	return true;
}


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



