/**
 * uplink.h
 *
 * Header files for uplink.c
 */

#ifndef __UPLINK__
#define __UPLINK__

#include <stdint.h>
#include <stdbool.h>
#include "../packet_types.h"

const int UPLINK_PACKET_LENGTH = 64;
const int UPLINK_PAYLOAD_LENGTH = 62;
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

struct uplink_packet {
	uint8_t opcode;
	uint8_t id;
	uint8_t payload[UPLINK_PAYLOAD_LENGTH];
} uplink_packet;


#ifdef SQUIRREL
const uint8_t uplink_packet_length = 255;
void uplink_init(void);
void uplink_send_configuration(uint8_t);
void uplink_receive_configuration(uint8_t);
void uplink_send_log(void);
void uplink_handle_packet((struct uplink_packet) *);
#endif
