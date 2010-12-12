/**
 * \brief Header files for uplink.c
 <pre>
 * Package format:
 * | OPCODE (1)  | ID (1)   | PAYLOAD (62) |
 *
 * LIST:
 *
 * | OPCODE (1)  | DEVICE_NUM (1) |
 * | BT ADDR (6) | NUT_CLASS (1)  |
 * | EXTENSION_CLASS_1 (1) | .... | TERMINATOR (1)
 *
 * LOG:
 *
 * | OPCODE (1)  | PAGE (1)       |
 * | BT ADDR (12) | EXTENSION_ID (1) |
 *
 * | LOG_TIME_1 (4) | VALUE_1 (2) |
 * .....
 * | LOG_TIME_8 (4) | VALUE_8 (2) | PAD (1)
 *
 * TIME:
 *
 * | OPCODE (1) | -- (1) | TIME (4) | -- (58) |
 </pre>
 *
 * @file uplink.h
 * @addtogroup Protocols
 */
 /**@{*/

#ifndef __UPLINK_H__
#define __UPLINK_H__

#include "../classes.h"
#include "../package_types.h"
#include "../../shared/common.h"

///uplink packet length define
#define UPLINK_PACKAGE_LENGTH 64
///uplink payload length define
#define UPLINK_PAYLOAD_LENGTH 62

///_uplink_payload_list struct datatype
typedef struct _uplink_payload_list   uplink_payload_list;
///_uplink_payload_log struct datatype
typedef struct _uplink_payload_log    uplink_payload_log;
///_uplink_payload_time struct datatype
typedef struct _uplink_payload_time   uplink_payload_time;
/// _uplink_package struct datatype
typedef struct _uplink_package        uplink_package;
/// _log_entry struct datatype
typedef struct _log_entry             log_entry;

/// log entry struct
struct _log_entry
{
  uint32_t time; ///< unixtimestamp of log entry
  uint16_t value; ///< logged value
};

/// payload list struct for list packets
struct _uplink_payload_list
{
  char bt_address[12]; ///< a device bluetooth address split into array
  uint8_t nut_class; ///< the according nut class
  uint8_t extension_class[49]; ///< all extension classes (max 49)
};

/// payload log struct for a log packets
struct _uplink_payload_log
{
  char bt_address[12]; ///< the device bluetooth address where the log comes from 
  uint8_t extension_id; ///< the extension id to the log value
  log_entry entries[8]; //< array of 8 log entrys (as a log entry struct)
  uint8_t padding; ///< just padding can be ignored (to force same size for all uplink packets)
};

/// paylod time struct for time packet
struct _uplink_payload_time
{
  uint32_t time; ///< unix timestamp with a time
  uint8_t padding[58]; ///< just padding can be ignored (to force same size for all uplink packets)
};

/// a general uplink payload type which can hold all possible packet formats
union payload
{
  uplink_payload_list list; ///< content format of packet for list requests
  uplink_payload_log log; ///< content format of packet for log requests
  uplink_payload_time time; ///< content format of packet for time requests
} payload;

///a general uplink packet with opcode id and payload
struct _uplink_package
{
  uint8_t opcode; ///< opcode of the packet
  uint8_t id; ///< id of packet for identification
  union payload payload; ///< payload with the data
};


#ifdef SQUIRREL

/**
* Handler for recieved data and parsing
* @param data buffer which holds the recieved data
* @return false if error occured in packet handling, true if succesful
*/
bool uplink_process_pkg (uint8_t * data);
#endif

#endif
/**@}*/
