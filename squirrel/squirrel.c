/*
 * squirrel.c
 *
 */

#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>
#include "squirrel.h"
#include "../drivers/bluetooth/bluetooth.h"
//#include "../drivers/storage/storage.h"
//#include "../drivers/display/display.h"
//#include "../drivers/platform/platform.h"
#include "../protocols/downlink/downlink.h"
#include "../protocols/uplink/uplink.h"
#include "../protocols/package_types.h"

/* -----------------------------------------------------------------------
 * Squirrel State
 * ----------------------------------------------------------------------- */

volatile uint8_t state;

extern uint8_t squirrel_state_get (void)
{
  return state;
}

extern void squirrel_state_set (uint8_t s)
{
#ifdef DEBUG
  switch (s)
    {
    case SLAVE:
      debug_pgm (PSTR ("Switching to SLAVE..."));
      break;

    case SLAVE_BUSY:
      debug_pgm (PSTR ("Switching to SLAVE_BUSY..."));
      break;

    case MASTER:
      debug_pgm (PSTR ("Switching to MASTER..."));
      break;

    default:
      debug_pgm (PSTR ("Switching to ???..."));
    }
#endif
  state = s;
}

/* -----------------------------------------------------------------------
 * Nut Handling
 * ----------------------------------------------------------------------- */

#define NUTS_LIST 16
#define EXTENSIONS_LIST 16
#define POLL_INTERVALL 15

typedef struct _device_info device_info;

struct _device_info
{
  uint8_t mac[6];
  uint8_t class;
  uint8_t extension_types[EXTENSIONS_LIST];
  uint16_t values_cache[EXTENSIONS_LIST];
};

static device_info device_list [NUTS_LIST];

bool squirrel_list (uint8_t num, uplink_payload_list *p)
{
  if (num < EXTENSIONS_LIST && device_list[num].mac[0] != 0)
    {

      for (int i = 0; i < 6; i++)
        p->bt_address[i] = device_list[num].mac[i];

      p->nut_class = device_list[num].class;

      for (int i = 0; i < EXTENSIONS_LIST; i++)
        p->extension_class[i] = device_list[num].extension_types[i];

      return true;
    }
  else
    {
      return false;
    }
}

void squirrel_create_device_info_entry (const uint8_t *address)
{
  // ??? debug bluetooth address
  for (int k = 0; k < NUTS_LIST; k++)
    {
      if (device_list[k].mac[0] == 0)
        {
          // We haven't found the MAC, time to create a new entry
          memcpy (&device_list[k], (void *) address, 6);

          device_list[k].class = downlink_get_nut_class (NULL);

#ifdef DEBUG

#endif
          for (int j = 0; j < 16; j++)
            {
              device_list[k].extension_types[j] = downlink_get_extension_class(j, NULL);
            }
          return;
        }
      else if (memcmp (device_list[k].mac, (void *) address, 6))
        {
          // Already there, nothing to do
          return;
        }
    }

  error ("Out of Memory");
}

void downlink_discover(void)
{
  char *found = bluetooth_cmd_search_devices();

  assert (found != NULL, "Malformed search result");

  uint8_t count = found[0];

  for (uint8_t i = 0; i < count; i++)
    {
      // Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
      squirrel_create_device_info_entry (&found[1 + i * (16 + 6) + 16]);
    }
}

void downlink_get_values (void)
{
  for (int k = 0; k < NUTS_LIST; k++)
    {
      if (device_list[k].mac[0] == 0)
        {

        }
    }

}

/* -----------------------------------------------------------------------
 * Init
 * ----------------------------------------------------------------------- */

void init_bluetooth (void)
{
  for (uint8_t i = 0; i < NUTS_LIST; i++)
    for (uint8_t j = 0; j < 6; j++)
      device_list[i].mac[j] = 0;

  bluetooth_init (uplink_bluetooth_callback_handler);
  uint8_t result = bluetooth_test_connection(4);
  assert (result == 1, "Could not test the connection");
  result = bluetooth_set_as_master();
  assert (result == 1, "Could not set master mode");
}

void init_display (void)
{

}

void init_storage (void)
{
  //fat16_init (0);
}

void init_downlink (void)
{
  downlink_discover();
}

void slave_loop (void)
{
  while (state == SLAVE)
    {
      // We wait for connections from the backend...
    }
}

void master_loop (void)
{
  // FIXME: Trim Strings!
  assert (bluetooth_set_as_master() == 1, "Could not set master mode");

  for (int k = 0; k < NUTS_LIST && device_list[k].mac[0] != 0; k++)
    {
      if (bluetooth_cmd_set_remote_address (&device_list[k].mac) != 1)
        break; // Could not connect

      for (int i = 0; i < EXTENSIONS_LIST; i++)
        {
          if (device_list[k].extension_types[i] < 0x80)
            {
              device_list[k].values_cache[i] = downlink_get_sensor_value(i, NULL);
              // log??
            }
        }
    }

  assert (bluetooth_set_as_slave() == 1, "Could not set master mode");
  state = SLAVE;
}

void draw_ui (uint8_t device, uint8_t page)
{
  const char *heading;

  switch (device_list[device].class)
    {
      // FIXME: Move to EEPROM!!
    case WEATHERSTATION:
      heading = "Wetterstation";
      break;

    default:
      heading = "Unbekannt";
      break;
    }

  // call display.h / widgets.c
}


int main (void)
{
  sei();
  init_bluetooth ();
  init_display ();
  init_storage ();
  init_downlink ();

  state = MASTER;

  while (true)
    {
      // Check for input events...
      // redraw UI
      // TIMER events!

      // if (something changed)
      draw_ui (0, 0);

      if (state == MASTER)
        {
          master_loop ();
        }

      if (state == SLAVE)
        {
          slave_loop ();
        }
    }
}

