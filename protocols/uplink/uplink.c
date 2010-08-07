/*
 *
 * uplink.c
 *
 * Uplink protocol implementation
 */

#include "uplink.h"

#ifdef SQUIRREL

#include "../squirrel/squirrel.h"
#include "../squirrel/util.h"
#include "../../drivers/bluetooth/bluetooth.h"

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

static inline bool uplink_handle_get_package (uplink_package *p)
{
  switch (p->opcode & 0x0F)
    {

    case LIST:
      return squirrel_list (p->id, &(p->payload.list));

    case LOG:
      return false;

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

void uplink_bluetooth_callback_handler (char *data_package, const uint8_t callback_type, const uint8_t data_length)
{
  bool error;
  uplink_package *p;

  if (callback_type != 0)
    return;


  if (data_length != UPLINK_PACKAGE_LENGTH)
    {
      error_pgm (PSTR ("Malformed Uplink package"));
      return;
    }

  squirrel_state_set (SLAVE_BUSY);

  p = (uplink_package *) data_package;

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
      error = false;
      break;

    case ECHO:
      error = false;
      break;

    default:
      error = true;
      break;
    }

  if (error)
    p->opcode = ERROR;
  else
    p->opcode = RET;

  bluetooth_send_data_package (&(p->opcode), UPLINK_PACKAGE_LENGTH);
}

#endif
