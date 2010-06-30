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
     
 *
 * uplink.h
 *
 * Header files for uplink.c
 */

#ifndef __UPLINK_H__
#define __UPLINK_H__

#include "../classes.h"
#include "../package_types.h"
#include "../../shared/common.h"

#define UPLINK_PACKAGE_LENGTH 64
#define UPLINK_PAYLOAD_LENGTH 62

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

typedef struct _uplink_payload_list   uplink_payload_list;
typedef struct _uplink_payload_log    uplink_payload_log;
typedef struct _uplink_payload_time   uplink_payload_time;
typedef struct _uplink_payload_tunnel uplink_payload_tunnel;
typedef struct _uplink_package        uplink_package;

struct _uplink_payload_list {
	uint8_t bt_address[6];
	uint8_t nut_class;
	uint8_t extension_class[55];
};

struct _uplink_payload_log {
	uint8_t bt_address[6];
	uint8_t extension_id;
	uint8_t extension_class;
	uint32_t log_time;
	uint16_t time_intervall;
	uint16_t values[24];
};

struct _uplink_payload_time {
	uint32_t time;
	uint8_t padding[58];
};

struct _uplink_payload_tunnel {
	uint8_t bt_address[6];
	uint8_t tunnel[4];
	uint8_t padding[52];
};

union payload {
	uplink_payload_list list;
	uplink_payload_log log;
	uplink_payload_time time;
	uplink_payload_tunnel tunnel;
} payload;

struct _uplink_package {
	uint8_t opcode;
	uint8_t id;
	union payload payload;
};


#ifdef SQUIRREL
void uplink_init(void);
void uplink_send_configuration(uint8_t);
void uplink_receive_configuration(uint8_t);
void uplink_send_log(void);
bool uplink_handle_package (uplink_package *p);
#endif
#endif // ENDS UPLINK
