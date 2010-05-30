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

#include "../classes.h"
#include "../package_types.h"

extern const int DOWNLINK_PACKAGE_LENGTH;
extern const int MAX_HISTORY; // FIXME!
/**
 * Every ID identifies a sensor/actor, but can also be used for config access with special flags.
 *
 * Package format:
 * | OPCODE (1) | ID (1) | VALUE (2) |
 */

typedef _downlink_package downlink_package;
struct _downlink_package {
	uint8_t opcode;
	uint8_t id;
	uint16_t value;
};

// Switch functionality 

#ifdef SQUIRREL
extern uint16_t downlink_get_sensor_value (uint8_t id, bool *err);
extern uint16_t downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err);
extern uint8_t  downlink_get_nut_class (bool *err);
extern uint8_t  downlink_get_actuator_class (uint8_t id, bool *err);
extern uint8_t  downlink_get_sensor_class (uint8_t id, bool *err);
extern uint16_t downlink_bye (uint16_t time_ms, bool *err);
#endif

#ifdef NUT

/**
 * MUST BE IMPLEMENTED IN YOUR FILES 
 */
extern const uint8_t class_id_nut;
extern const uint8_t class_id_extensions[];
extern const uint8_t class_id_extensions_length;

extern uint16_t get_value(uint8_t id);
extern void set_value(uint8_t id, uint16_t value);

/**
 * Returns true if the package was handled successfully and the buffer can be returned, false otherwise
 */
bool downlink_handle_package ( struct downlink_package* );

#endif
#ifdef SERIAL
bool downlink_handle_package ( struct downlink_package* );
#endif
#endif
