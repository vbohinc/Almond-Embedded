/**
 * uplink.c
 *
 * Uplink protocol implementation
 */

#include "uplink.h"
 
const int UPLINK_PACKAGE_LENGTH = 64;
const int UPLINK_PAYLOAD_LENGTH = 62;

#ifdef SQUIRREL

/**
 * Handle GET package
 */
static inline bool uplink_handle_get_package (downlink_package *p) {
	switch (p->opcode & 0x0F) {
	
		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] < GENERIC_ACTOR)
				p->value = get_value (p->id);
			else 
			  return false;
			break;
			
		case INFO_NUT:
			p->id = 0;
			p->value = class_id_nut;
			break;
			
		case INFO_EXTENSION:
			if (p->id >= class_id_extensions_length) return false;
			p->value = class_id_extensions[p->id];
			break;
			
		case CONFIG:
			return false;
			
		default:
			return false;
	}
	
	return true;
}

/**
 * Handle SET packages
 */
static inline bool uplink_handle_set_package (downlink_package *p) {
	switch (p->opcode & 0x0F) {

		case STANDARD:
			if (p->id < class_id_extensions_length && class_id_extensions[p->id] >= GENERIC_ACTOR)
				set_value (p->id, p->value);
			else
				return false;
			break;
			
		case CONFIG:
			return false;
			
		default:
			return false;
	}
	
	return true;
}

/* FIXME: Change parameter list */
void uplink_bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
  bool return_package;
  uplink_package *p;

	if (callback_type != 0 && data_length != UPLINK_PACKAGE_LENGTH) {
	  error ("Malformed Downlink package");
	  return;
	}
	
	p = (uplink_package) data_package; 
	
	switch (p->opcode & 0xF0) {
	
	  case GET:
		  return_package = uplink_handle_get_package (p);
		  break;

		case SET:
			return_package = uplink_handle_set_package (p);
			break;

		case TUNNEL:
			// FIXME! bluetooth_disabled_for_s = p->value;
			p->id = 0;
			p->value = 0;
			return_package = true;
      break;
      
		case ECHO:
			return_package = true;
      break;
      
		default:
			return_package = false;
			break;
	}
	
	if (return_package) {
	  p->opcode = RET;
	  bluetooth_send_data_package (p, UPLINK_PACKAGE_LENGTH, false, 100);
	}
}

#endif
