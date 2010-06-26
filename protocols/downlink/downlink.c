/**
 * downlink.c
 *
 * Downlink protocol implementation
 */

#include "downlink.h"
#include "../../drivers/bluetooth/bluetooth.h"

#ifdef SQUIRREL

static downlink_package package;

/* WARNING: Assuming layer above already connected */
uint16_t downlink_request (uint8_t opcode, uint8_t flag, uint8_t id, uint16_t value, bool *err) {
  
  //downlink_package *package = malloc(sizeof(downlink_package));
  
  err = false;
  package.opcode = opcode | flag;
  package.id = id;
  package.value = value;
  uint8_t return_package_length;
  return_package_length = DOWNLINK_PACKAGE_LENGTH;
  
  switch (bluetooth_send_data_package(&package, &return_package_length, true, DOWNLINK_TIMEOUT_MS)) {
    case 0:
      if ((package.opcode == RET | flag) && (package.id == id))
        return package.value;
      else if (package.opcode == (RET | ERROR))
        error ("Protocol error");
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
  
  err = true;
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
static inline bool downlink_handle_set_package (downlink_package *p) {
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
void downlink_bluetooth_callback_handler (char *data_package, const uint8_t callback_type, const uint8_t data_length)
{
  bool return_package;
  downlink_package *p;

	if (callback_type != 0 && data_length != DOWNLINK_PACKAGE_LENGTH) {
	  error ("Malformed Downlink package");
	  return;
	}
	
	p = (downlink_package *) data_package; 
	
	switch (p->opcode & 0xF0) {
	
	  case GET:
		  return_package = downlink_handle_get_package (p);
		  break;

		case SET:
			return_package = downlink_handle_set_package (p);
			break;

		case BYE:
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
          //FIXME won't work length needs to be a pointer, which to my viewpoint is the worst possible idea ever..... - Linus
	  bluetooth_send_data_package (p, DOWNLINK_PACKAGE_LENGTH, false, 100);
	}
}
#endif
