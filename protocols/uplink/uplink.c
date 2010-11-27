/**
 * uplink.c - Uplink (PC->Squirrel)
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#include "uplink.h"

#ifdef SQUIRREL

#include <squirrel/squirrel.h>
#include <squirrel/util.h>
#include <bluetooth/bluetooth.h>

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
 * | BT ADDR (12) | EXTENSION_ID (1) |
 *
 * | LOG_TIME_1 (4) | VALUE_1 (2) |
 * .....
 * | LOG_TIME_8 (4) | VALUE_8 (2) | PAD (1)
 *
 * TIME:
 *
 * | OPCODE (1) | -- (1) | TIME (4) | -- (58) |
 *
 */

static inline bool uplink_handle_get_package (uplink_package *p)
{
    switch (p->opcode & 0x0F)
    {

        case LIST:
            return squirrel_list (p->id, & (p->payload.list));

        case LOG:
            return squirrel_log (p);

        case TIME:
            p->payload.time.time = time_get ();
            return true;

        default:
            return true;
    }
}

static inline bool uplink_handle_set_package (uplink_package *p)
{
    switch (p->opcode & 0x0F)
    {

        case TIME:
            return !time_set (p->payload.time.time);

        default:
            return false;
    }
}

bool uplink_process_pkg (uint8_t * data, void (*menu_slave) (bool))
{
    bool error;
    uplink_package *p;

	if (menu_slave)
		menu_slave(true);
    squirrel_state_set (SLAVE_BUSY);
    p = (uplink_package *) data;

    switch (p->opcode & 0xF0)
    {

        case GET:
            error = uplink_handle_get_package (p);
            break;

        case SET:
            error = uplink_handle_set_package (p);
            break;

        case BYE:
            squirrel_state_set (SLAVE);
			if (menu_slave)
				menu_slave(false);
            error = false;
            break;

        case ECHO:
            error = false;
            break;

        default:
            error = true;
            break;
    }

    p->opcode |= error ? ERROR : RET;

    return bt_send ( (void *) p, UPLINK_PACKAGE_LENGTH);
}

#endif
