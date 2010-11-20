/**
 * uplink.h - Uplink (PC->Squirrel)
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
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
 * | BT ADDR (6) | EXTENSION_ID (1) |
 *
 * | LOG_TIME_1 (4) | VALUE_1 (2) |
 * .....
 * | LOG_TIME_9 (4) | VALUE_9 (2) | PAD (1)
 *
 * TIME:
 *
 * | OPCODE (1) | -- (1) | TIME (4) | -- (58) |
 *
 */

typedef struct _uplink_payload_list   uplink_payload_list;

typedef struct _uplink_payload_log    uplink_payload_log;

typedef struct _uplink_payload_time   uplink_payload_time;

typedef struct _uplink_package        uplink_package;

typedef struct _log_entry             log_entry;

struct _log_entry
{
    uint32_t time;
    uint16_t value;
};

struct _uplink_payload_list
{
    uint8_t bt_address[6];
    uint8_t nut_class;
    uint8_t extension_class[55];
};

struct _uplink_payload_log
{
    uint8_t bt_address[6];
    uint8_t extension_id;
    log_entry entries[9];
    uint8_t padding;
};

struct _uplink_payload_time
{
    uint32_t time;
    uint8_t padding[58];
};

union payload
{
    uplink_payload_list list;
    uplink_payload_log log;
    uplink_payload_time time;
} payload;

struct _uplink_package
{
    uint8_t opcode;
    uint8_t id;
    union payload payload;
};


#ifdef SQUIRREL
bool uplink_process_pkg (uint8_t * data, uint8_t length);
#endif

#endif
