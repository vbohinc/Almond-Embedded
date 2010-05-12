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

/**
 *
 *
 * Package format:
 * | OPCODE (1) | ID (1) | VALUE (2) |
 */

typedef struct downlink_packet {
	uint8_t opcode;
	uint8_t id;
	uint16_t value;
} downlink_packet;

#ifdef SQUIRREL
const uint8_t uplink_packet_length = 255;
void uplink_init(void);
void uplink_send_configuration(uint8_t device_id);
void uplink_receive_configuration(uint8_t device_id);
void uplink_send_log(void);

#endif
