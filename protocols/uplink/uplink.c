/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
      ___           ___                                   ___           ___     
     /__/\         /  /\                    ___          /__/\         /__/|    
     \  \:\       /  /::\                  /  /\         \  \:\       |  |:|    
      \  \:\     /  /:/\:\  ___     ___   /  /:/          \  \:\      |  |:|    
  ___  \  \:\   /  /:/~/:/ /__/\   /  /\ /__/::\      _____\__\:\   __|  |:|    
 /__/\  \__\:\ /__/:/ /:/  \  \:\ /  /:/ \__\/\:\__  /__/::::::::\ /__/\_|:|____
 \  \:\ /  /:/ \  \:\/:/    \  \:\  /:/     \  \:\/\ \  \:\~~\~~\/ \  \:\/:::::/
  \  \:\  /:/   \  \::/      \  \:\/:/       \__\::/  \  \:\  ~~~   \  \::/~~~~ 
   \  \:\/:/     \  \:\       \  \::/        /__/:/    \  \:\        \  \:\     
    \  \::/       \  \:\       \__\/         \__\/      \  \:\        \  \:\    
     \__\/         \__\/                                 \__\/         \__\/    
     
/**
 * uplink.c
 *
 * Uplink protocol implementation
 */

#include "uplink.h"
//#include "../../drivers/bluetooth/bluetooth.h"

#ifdef SQUIRREL

/**
 * Package format:
 * | OPCODE (1) | ID (1) | PAYLOAD (62) |
 *
 * LIST:
 *
 * | OPCODE (1)  | PAGE (1) |
 * | BT ADDR (6) | NUT_CLASS (1) | EXTENSION_CLASS_1 (1) | .... | TERMINATOR (1)
 * .....
 * | BT ADDR (6) | NUT_CLASS (1) | EXTENSION_CLASS_1 (1) | .... | TERMINATOR (1)
 *
 * LOG:
 *
 * | OPCODE (1) | PAGE (1) | BD ADDR (6)  |
 * | EXTENSION_ID (1) | EXTENSION_CLASS (1) | LOG_TIME (4) | TIME_INTERVALL (2)
 * | VALUE1 (2) | .... 	| VALUE4 (2) |
 * .....
 * | VALUE20 (2) | .... 	| VALUE24 (2) |
 *
 * TIME:
 *
 * | OPCODE (1) | -- (1) | TIME (4) | -- (58) |
 *
 * TUNNEL:
 *
 * | OPCODE (1) | -- (1)  | BD ADDR (6)  |
 * | DATA   (4) | -- (52) |
 */

/**
 * Handle GET package
 */
static inline bool uplink_handle_get_package (uplink_package *p) {
	switch (p->opcode & 0x0F) {
	  
	  case LIST:
	
		case LOG:
			// Log access?!
			return false;
			
		case TIME:
			// Time ?
			break;
			
		case TUNNEL:
			return false;
			
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
static inline bool uplink_handle_set_package (uplink_package *p) {
	switch (p->opcode & 0x0F) {

		case TIME:
			// Time ?
			break;
		
		case TUNNEL:
			return false;
			
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

	if ((callback_type != 0) && (data_length != UPLINK_PACKAGE_LENGTH)) {
	  error ("Malformed Uplink package");
	  return;
	}
	
	p = (uplink_package *) data_package; 
	
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
			//p->value = 0;
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
