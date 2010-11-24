/**
 * downlink.c - Downlink (Nut->Squirrel/Nut->PC)
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#include "downlink.h"
#include <bluetooth/bluetooth.h>

#ifdef SQUIRREL

void dump (downlink_package *p) 
{
	byte_to_hex (((char *)p)[0]);
	byte_to_hex (((char *)p)[1]);
	byte_to_hex (((char *)p)[2]);
	byte_to_hex (((char *)p)[3]);
	error_putc('\n');
}

/* WARNING: Assuming layer above already connected */
static uint16_t downlink_request (uint8_t opcode, uint8_t flag, uint8_t id, uint16_t value, bool *err)
{
    downlink_package package;
    uint8_t length;

    package.opcode = opcode | flag;
    package.id = id;
    package.value = value;
    *err = false;
    length = DOWNLINK_PACKAGE_LENGTH;

	dump (&package);

    if (!bt_send ( (void *) &package, length))
	{
		error_pgm PSTR("send failed");
	}
    else if (bt_receive ( (void *) &package, length, DOWNLINK_TIMEOUT_MS))
    {
		dump (&package);
		if ( (package.opcode == (RET | opcode | flag)) && (package.id == id))
			return package.value;
		else
			if (package.opcode == (ERROR | flag))
				error_pgm (PSTR ("Nut signalled error")); //
			else
				error_pgm (PSTR ("Downlink protocol mismatch")); //
	}

    else
    {
        error_pgm (PSTR ("receive error"));
    }

    *err = true;

    return 0;
}

uint16_t downlink_get_sensor_value (uint8_t id, bool *err)
{
    return downlink_request (GET, STANDARD, id, 0, err);
}

void downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err)
{
    downlink_request (SET, STANDARD, id, value, err);
}

uint8_t downlink_get_nut_class (bool *err)
{
    return downlink_request (GET, INFO_NUT, 0, 0, err);
}

uint8_t downlink_get_extension_class (uint8_t id, bool *err)
{
    return downlink_request (GET, INFO_EXTENSION, id, 0, err);
}

bool downlink_is_nut (bool *err)
{
    return 42 == downlink_request (ECHO, STANDARD, 0, 42, err);
}

void downlink_bye (uint16_t time_sec, bool *err)
{
    downlink_request (BYE, STANDARD, 0, time_sec, err);
}

#endif

#ifdef NUT

/**
 * Handle GET package
 */
static inline bool downlink_handle_get_package (downlink_package *p)
{
    switch (p->opcode & 0x0F)
    {

        case STANDARD:

            if (p->id < class_id_extensions_length && class_id_extensions[p->id] < GENERIC_ACTOR)
                p->value = get_value (p->id);
            else
                return false;

            break;

        case INFO_NUT:
            p->id = 0;

            p->value = class_id_nut;

            break;

        case INFO_EXTENSION:
            if (p->id >= class_id_extensions_length)
                return false;

            p->value = class_id_extensions[p->id];

            break;

        default:
            return false;
    }

    return true;
}

/**
 * Handle SET packages
 */
static inline bool downlink_handle_set_package (downlink_package *p)
{
    switch (p->opcode & 0x0F)
    {

        case STANDARD:

            if (p->id < class_id_extensions_length && class_id_extensions[p->id] >= GENERIC_ACTOR)
                set_value (p->id, p->value);
            else
                return false;

            break;

        default:
            return false;
    }

    return true;
}

bool downlink_process_pkg (uint8_t * data)
{
    bool return_package;
    downlink_package *p;

    p = (downlink_package *) (data);

    sleep = 0;

    switch (p->opcode & 0xF0)
    {

        case GET:
            return_package = downlink_handle_get_package (p);
            break;

        case SET:
            return_package = downlink_handle_set_package (p);
            break;

        case BYE:
            sleep = p->value;
            p->id = 0;
            p->value = 0;
            return_package = true;
            break;

        case ECHO:
            return_package = true;
            break;

        default:
            return_package = false;
            break;
    }

    p->opcode |= return_package ? RET : ERROR;

    return bt_send ( (void *) p, DOWNLINK_PACKAGE_LENGTH);
}

#endif
