/**
 * uplink.c
 *
 * Uplink protocol implementation
 */
const int UPLINK_PACKAGE_LENGTH = 64;
const int UPLINK_PAYLOAD_LENGTH = 62;
#include <stdbool.h>
#include "uplink.h"
#ifdef SQUIRREL

/**
 * Handle GET package
 */
static inline bool uplink_handle_get_package  (struct uplink_package *p) {
	/*switch (p->opcode & 0x0F) {
		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] < GENERIC_ACTOR) {
				p->opcode = RET;
				//p->value = get_value (p->id);
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
				//p->value = class_id_extensions[];
				return true;
			} else {
				return false;
			}
		case CONFIG:
			return false;
		default:
			return false;
	}*/
	return true;
}

/**
 * Handle SET packages
 */
static inline bool uplink_handle_set_package (struct uplink_package *p) {
/*	switch (p->opcode & 0x0F) {
		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] >= GENERIC_ACTOR) {
				p->opcode = RET;
				FIXME: Return Value??
				set_value (p->id, p->value);
				return true;
			} else {
				return false;
			}
		case CONFIG:
			return false;
		default:
			return false;
	}*/
	return true;
}

/**
 * Major package uplink handling function
 */
bool uplink_handle_package (struct uplink_package *p) {
	switch (p->opcode & 0xF0) {
		case GET:
			return uplink_handle_get_package (p);
		case SET:
			return uplink_handle_set_package (p);
		case TUNNEL:
			//return uplink_handle_tunnel_package (p);
		case ECHO:
			return true;
		default:
			return false;
	}
}
#endif
