#include <stdint.h>
#include <stdbool.h>
#include "downlink.h"

const uint8_t DOWNLINK_TIMEOUT_MS = 100;
const uint8_t DOWNLINK_PACKAGE_LENGTH = 4;

/* FIXME: const int MAX_HISTORY = 255; */
#ifdef SQUIRREL

/* WARNING: Assuming layer above already connected */
uint16_t downlink_request (uint8_t opcode, uint8_t flag, uint8_t id, uint16_t value, bool *err) {
  
  downlink_package *package;
  
  err = FALSE;
  package->opcode = opcode | flag;
  package->id = id;
  package->value = value;
  
  switch (bluetooth_send_data_package (package, DOWNLINK_PACKAGE_LENGTH, TRUE, const uint16_t timeout_ms)) {
    case 0:
      if (package->opcode == (RET | flag) && package->id == id) 
        return package->value;
      else if (package->opcode == (RET | ERROR)
        error ("Protocol error")
      else 
        error ("Downlink protocol mismatch");
      break;
    case 1:
      error ("Bluetooth error");
      break;
    case 2:
      warn ("Timeout");
      break;
    default:
      error ("Unkown return value");
  }
  err = TRUE;
  return 0;
}

uint16_t downlink_get_sensor_value (uint8_t id, bool *err) {
  return downlink_request (GET, STANDARD, id, 0, err);
}

uint16_t downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err) {
  return downlink_request (SET, STANDARD, id, value, err);
}

uint8_t downlink_get_nut_class (bool *err) {
  return (uint8_t) downlink_request (GET, INFO_NUT, 0, 0, err);
}

uint8_t downlink_get_actuator_class (uint8_t id, bool *err) {
  return (uint8_t) downlink_request (GET, INFO_EXTENSION, id + 0x80, 0, err);
}

uint8_t downlink_get_sensor_class (uint8_t id, bool *err) {
  return (uint8_t) downlink_request (GET, INFO_EXTENSION, id, 0, err);
}

uint16_t downlink_bye (uint16_t time_ms, bool *err) {
  return downlink_request (BYE, 0, 0, time_ms, err);
}
#endif

#ifdef NUT

/**
 * Handle GET package
 */
static inline bool downlink_handle_get_package (downlink_package *p) {
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
static inline bool downlink_handle_set_package (downlink_package *p) {
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

/* FIXME: Change parameter list */
void downlink_bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {
		// Data package
		if (data_length == DOWNLINK_PACKAGE_LENGTH) {
			downlink_handle_package_really ((downlink_package) data_package);
		} 
	} else if (callback_type == 1) {
		// Connect
	} else if (callback_type == 2) {
		// Disconnect
	}
}


/**
 * Major downlink package handling function
 */
bool downlink_handle_package_really (downlink_package *p) {
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

bool downlink_handle_package (downlink_package *p) {
	return true;
}
