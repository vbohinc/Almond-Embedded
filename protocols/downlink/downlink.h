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
#include "../packet_types.h"
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
} downlink_packet;


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
