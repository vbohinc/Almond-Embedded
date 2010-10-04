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
 * uint16_t sleep;
 * uint16_t get_value(uint8_t id);
 * void set_value(uint8_t id, uint16_t value);
 */

#ifndef __DOWNLINK_H__
#define __DOWNLINK_H__

#include "../classes.h"
#include "../package_types.h"
#include "../../shared/common.h"

/* Protocol Details */

#define DOWNLINK_TIMEOUT_MS 100
#define DOWNLINK_PACKAGE_LENGTH 4

/**
 * Every ID identifies a sensor/actor.
 *
 * Package format:
 * | OPCODE (1) | ID (1) | VALUE (2) |
 */

typedef struct _downlink_package downlink_package;

struct _downlink_package
{
  uint8_t opcode;
  uint8_t id;
  uint16_t value;
};

#ifdef SQUIRREL

extern uint16_t downlink_get_sensor_value (uint8_t id, bool *err);
extern void     downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err);
extern uint8_t  downlink_get_nut_class (bool *err);
extern uint8_t  downlink_get_extension_class (uint8_t id, bool *err);
extern void     downlink_bye (uint16_t time_sec, bool *err);

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
extern uint16_t sleep;

/* Callback handler */
extern void downlink_bluetooth_callback_handler (char *data_package, const uint8_t callback_type, const uint8_t data_length);

#endif

#endif
