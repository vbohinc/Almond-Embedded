/**
 * downlink.h
 * 
 * Header files for downlink.c
 *
 * Everybody using this header for a nut must provide following:
 *
 * const uint8_t class_id_nut;
 * const uint8_t class_id_extensions[];
 * const uint8_t class_id_extensions_length;
 */

#ifndef __DOWNLINK__
#define __DOWNLINK__

#include <stdint.h>
#include <stdbool.h>
#include "../packet_types.h"
#include "../classes.h"

const int DOWNLINK_PACKET_LENGTH = 4;

/**
 * Every ID identifies a sensor/actor, but can also be used for config access with special flags.
 *
 * Package format:
 * | OPCODE (1) | ID (1) | VALUE (2) |
 */

struct downlink_packet {
	uint8_t opcode;
	uint8_t id;
	uint16_t value;
};

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
