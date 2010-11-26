/**
 * downlink.c - Downlink (Nut->Squirrel/Nut->PC)
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 *
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

uint16_t downlink_get_sensor_value (uint8_t id, bool *err);
void     downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err);
uint8_t  downlink_get_nut_class (bool *err);
uint8_t  downlink_get_extension_class (uint8_t id, bool *err);
bool     downlink_is_nut (bool *err);
void     downlink_bye (uint16_t time_sec, bool *err);

#endif

#ifdef NUT

/**
 * MUST BE IMPLEMENTED IN YOUR FILES
 */

extern const uint8_t class_id_nut;
extern const uint8_t class_id_extensions[];
extern const uint8_t class_id_extensions_length;
extern uint16_t get_value (uint8_t id);
extern void set_value (uint8_t id, uint16_t value);
extern uint16_t sleep;

/* Callback handler */
bool downlink_process_pkg (uint8_t * data);

#endif

#endif
