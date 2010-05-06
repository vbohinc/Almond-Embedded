#include <stdint.h>
#include <stdbool.h>
#include "downlink.h"
#include "packet_types.h"
#include "../classes.h"


void downlink_discover () {
	return;
}


static bool get_package (struct downlink_packet *p) {

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

static bool set_package (struct downlink_packet *p) {
	return true;
}


bool downlink_handle_package (struct downlink_packet *p) {
	switch (p->opcode & 0xF0) {
		case GET:
			return get_package (p);
		case SET:
			return set_package (p);
		case ECHO:
			return true;
		default:
			return false;
	}
}




