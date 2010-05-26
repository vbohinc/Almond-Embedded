#include <stdint.h>
#include <stdbool.h>
#include "downlink.h"

const int DOWNLINK_PACKAGE_LENGTH = 4;
const int MAX_HISTORY = 255;
#ifdef SQUIRREL

void downlink_discover () {
	return;
}


/**
 * Handle RET package
 */
static bool downlink_handle_ret_package (struct downlink_package *p) {

	// Utility functions
	/*switch (p->opcode & 0x0F) {
		case INFO_NUT:
		if (!find_nut_by_MAC(p)) {
			// Issue new running number
			// Insert into mac_lookup
			// Insert into class_lookup
		}
		case INFO_EXTENSION:
		// Determine running number
		// Insert into relevant lookup table

	}*/
	return true;
}

uint16_t downlink_get_sensor (uint8_t class, uint8_t id, struct downlink_package *p) {
  p->opcode = GET;
  p->id = id;
  p->value = 0;
  
  //bluetooth_handle_package (p);
  
  if (p->opcode == 0xF0 && p->id == id) { // ECHO
    return p->value;
  } else {
    return -1;
  }  
}

uint8_t downlink_get_class () {
	struct downlink_package *p;
	
	p->opcode = GET | INFO_NUT;
	p->id = NULL;
	p->value = NULL;
	
	bluetooth_handle_package (p);
	
	if (p->opcode == RET | INFO_NUT) {
		return p-> value;
	} else {
		return -1;
	}
}

uint8_t downlink_get_actuator_class (uint8_t id) {
	return _get_extension_class(id);
}

uint8_t downlink_get_sensor_class (uint8_t id) {
	return _get_extension_class(id);
}

uint8_t _get_extension_class (uint8_t id) {
	struct downlink_package *p;
	
	p->opcode = GET | INFO_EXTENSION;
	p->id = id;
	p->value = NULL;
	
	bluetooth_handle_package (p);
	if (p->opcode == RET | INFO_EXTENSION) {
		return p->value;
	} else {
		return -1;
	}
}

#endif

#ifdef NUT

/**
 * Handle GET package
 */
static inline bool downlink_handle_get_package (struct downlink_package *p) {
	switch (p->opcode & 0x0F) {
		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] < GENERIC_ACTOR) {
				p->opcode = RET;
				if (0 == p->value ) {
					p->value = get_value (p->id);
				} else if (0 < p->value && p->value <= MAX_HISTORY) {
					//FIXME! p->value = get_cached_value (p->id, p->value);
				} else {
					return false;
				}
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
				p->value = class_id_extensions[p->id];
				return true;
			} else {
				return false;
			}
		/* Not supported yet */
		case CONFIG:
			return false;
		default:
			return false;
	}
}

/**
 * Handle SET packages
 */
static inline bool downlink_handle_set_package (struct downlink_package *p) {
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
		/* Not supported yet */
		case CONFIG:
			return false;
		default:
			return false;
	}
}


/**
 * Major downlink package handling function
 */
bool downlink_handle_package (struct downlink_package *p) {
	switch (p->opcode & 0xF0) {
		case GET:
			return downlink_handle_get_package (p);
		case SET:
			return downlink_handle_set_package (p);
		/* FIXME: Should we return something. Maybe a GLOBAL Bluetooth activated? */
		case BYE:
			// FIXME! bluetooth_disabled_for_s = p->value;
			p->opcode = RET;
			p->id = 0;
			p->value = 0;
			return true;
		case ECHO:
			return true;
		default:
			return false;
	}
}
#endif

bool downlink_handle_package (struct downlink_package *p) {
	return true;
}
