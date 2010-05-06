/**
 * 
 * downlink.h
 * Header for downlink - The Squirrel-Nuts Communication Protocol
 *
 * Everybody using this header must provide following:
 *
 * const uint8_t class_id_nut;
 * const uint8_t class_id_sensors[128];
 * const uint8_t class_id_actors[128];
 */


#ifndef __DOWNLINK__
#define __DOWNLINK__

#include <stdint.h>
#include <stdbool.h>

const int DOWNLINK_PACKET_LENGTH = 4;


/**
 * Every ID identifies a sensor/actor, but can also be used for config access with special flags.
 *
 * Package format:
 * | OPCODE (1) | ID (1) | VALUE (2) |
 */
 
typedef struct downlink_packet {
	uint8_t 	opcode; 
	uint8_t		id;
	uint16_t	value;
};

enum downlink_packet_type {
	GET		= 0x10,		// Used by: Squirrel
	SET		= 0x20,     // Used by: Squirrel                                                   
	RET		= 0x30, 	// Used by: Nut
	ECHO	= 0xF0,		// Used for debug, returns the packet
};

enum downlink_packet_type_flags {
	STANDARD = 0x00,	// Standard modifier

	INFO_NUT = 0x01,	// Get the class of the Nut 
	INFO_ID  = 0x02,    // Get the class of the Sensor/Actor
	CONFIG	 = 0x0A,    // Access the configuration
	ADVANCED = 0x0F,	// Advanced features
}

// Switch functionality 

#ifdef SQUIRREL

	/**
	 * Discover nuts, returns ???
	 */
	
	void downlink_discover ();
	
	/**
	 *
	 */
	 
#endif

#ifdef NUT

	/**
	 * Returns true if the package was handled successfully and the buffer can be returned, false otherwise
	 */
	bool downlink_handle_package ( *(struct downlink_packet) );
	
#endif


#endif
