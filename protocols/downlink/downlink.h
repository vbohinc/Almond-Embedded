/**
 * \brief Header files for downlink protocol
 * Everybody using this header for a nut must provide following:
 * <pre> 
 * const uint8_t class_id_nut;
 * const uint8_t class_id_extensions[];
 * const uint8_t class_id_extensions_length;
 * uint16_t sleep;
 * uint16_t get_value(uint8_t id);
 * void set_value(uint8_t id, uint16_t value);
 * </pre>
 * @file downlink.h
 * @addtogroup Protocols
 */
 
/**@{*/

#ifndef __DOWNLINK_H__
#define __DOWNLINK_H__

#include "../classes.h"
#include "../package_types.h"
#include "../../shared/common.h"

/* Protocol Details */

///downlink communication timeout
#define DOWNLINK_TIMEOUT_MS 100
///downlink packet length
#define DOWNLINK_PACKAGE_LENGTH 4

/**
 * Every ID identifies a sensor/actor.
 *<pre>
 * Package format:
 * | OPCODE (1) | ID (1) | VALUE (2) |
 </pre>
 * hold in this downlink_package struct datatype
 */

typedef struct _downlink_package downlink_package;

struct _downlink_package
{
  uint8_t opcode; ///< opcode of packet
  uint8_t id; ///< id 
  uint16_t value; ///< value
};

#ifdef SQUIRREL

/**
* Gets value of sensor with of given id
* @param id of the sensor to request value of
* @param err true if error occured, false if not
* @return returns value of sensor, if err occured always 0   
*/
uint16_t downlink_get_sensor_value (uint8_t id, bool *err);

/**
*Sets actor value of given id
* @param id of the actor to set value
* @param value to set actor to
* @param err true if error occured, false if not
*/
void     downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err);

/**
* Gets nut class of the connected download device
* @param err true if error occured, false if not
* @return nut class or if err is true always 0
*/
uint8_t  downlink_get_nut_class (bool *err);

/**
* Gets extension class to id of device extension
* @param id of the extension
* @param err true if error occured, false if not
* @return extension class or if err is true always 0
*/
uint8_t  downlink_get_extension_class (uint8_t id, bool *err);

/**
* Ask if connected device is Nut
* @para err true if error occured, false if not
* @return true if nut, false if not or err is true
*/
bool     downlink_is_nut (bool *err);

/**
* send a bye packet to close the downlink communication and let the device sleep to save energy for given time
* @param time_sec time in seconds for the device to go into sleep
* @para err true if error occured, false if not
*/
void     downlink_bye (uint16_t time_sec, bool *err);

#endif

#ifdef NUT



extern const uint8_t class_id_nut; ///< MUST BE IMPLEMENTED IN YOUR FILES
extern const uint8_t class_id_extensions[]; ///< MUST BE IMPLEMENTED IN YOUR FILES
extern const uint8_t class_id_extensions_length; ///< MUST BE IMPLEMENTED IN YOUR FILES
extern uint16_t get_value (uint8_t id); ///< MUST BE IMPLEMENTED IN YOUR FILES
extern void set_value (uint8_t id, uint16_t value); ///< MUST BE IMPLEMENTED IN YOUR FILES
extern uint16_t sleep; ///< MUST BE IMPLEMENTED IN YOUR FILES

/** Callback handler for packet processing*/
bool downlink_process_pkg (uint8_t * data, uint8_t length);

#endif

#endif
/**@}*/
