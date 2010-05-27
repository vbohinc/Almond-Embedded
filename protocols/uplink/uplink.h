/**
 * uplink.h
 *
 * Header files for uplink.c
 */

#ifndef __UPLINK__
#define __UPLINK__

#include <stdint.h>
#include <stdbool.h>
#include "../package_types.h"

extern const int UPLINK_PACKAGE_LENGTH;
extern const int UPLINK_PAYLOAD_LENGTH;
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

struct uplink_payload_list {
	uint8_t bt_address[6];
	uint8_t nut_class;
	uint8_t extension_class[55];
};

struct  uplink_payload_log {
	uint8_t bt_address[6];
	uint8_t extension_id;
	uint8_t extension_class;
	uint32_t log_time;
	uint16_t time_intervall;
	uint16_t values[24];
};

struct  uplink_payload_time {
	uint32_t time;
	uint8_t padding[58];
};

struct  uplink_payload_tunnel {
	uint8_t bt_address[6];
	uint8_t tunnel[4];
	uint8_t padding[52];
};

union payload {
	struct uplink_payload_list list;
	struct uplink_payload_log log;
	struct uplink_payload_time time;
	struct uplink_payload_tunnel tunnel;
} payload;

struct uplink_package {
	uint8_t opcode;
	uint8_t id;
	union payload payload;
};


#ifdef SQUIRREL
void uplink_init(void);
void uplink_send_configuration(uint8_t);
void uplink_receive_configuration(uint8_t);
void uplink_send_log(void);
bool uplink_handle_package (struct uplink_package *p);
#endif
#endif // ENDS UPLINK
