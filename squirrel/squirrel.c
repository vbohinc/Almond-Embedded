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


#define NUTS_LIST 16

typedef struct _device_info device_info;

struct _device_info
{
  uint8_t mac[6];
  uint8_t class;
  uint8_t actuator_types[16];
  uint8_t sensor_types[16];
  uint8_t config_types[16];
};


const uint8_t poll_time EEMEM = 15;

static uint8_t state;
static device_info device_list [NUTS_LIST];

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

    case defaul:
      debug_pgm (PSTR ("Switching to ???..."))
    }
#endif
  state = s;
}

void squirrel_create_device_info_entry (const uint8_t *address)
{
#ifdef DEBUG

#endif
  for (int k = 0; k < NUTS_LIST; k++)
    {
      if (device_list[k].mac[0] == 0)
        {
          // We haven't found the MAC, time to create a new entry
          memcpy (&device_list[k], (void *) address, 6);

          device_list[k].class = downlink_get_nut_class (NULL);

          for (int j = 0; j < 16; j++)
            {
              device_list[k].sensor_types[j] = downlink_get_sensor_class(j, NULL);
              device_list[k].actuator_types[j] = downlink_get_actuator_class(j + 0x80, NULL); // Actuators begin at ID 0x80
              // TODO config_types
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
  uint8_t *found = bluetooth_cmd_search_devices();

  assert (found != NULL, "Malformed search result");

  int count = found[0];

  for (int i = 0; i < count; i++)
    {
      // Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
      squirrel_create_device_info_entry (&found[1 + i * (16 + 6) + 16]);
    }

}

void bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
  if (callback_type == 0)
    {
      // Data package
      if (data_length == DOWNLINK_PACKAGE_LENGTH)
        {
          //downlink_handle_package(data_package);
        }
      else if (data_length == UPLINK_PACKAGE_LENGTH)
        {
          //	uplink_handle_package(data_package);
        }
    }
  else if (callback_type == 1)
    {
      // Connect
    }
  else if (callback_type == 2)
    {
      // Disconnect
    }
}

void init_bluetooth (void)
{
  // Mem Clean
  for (int i = 0; i < NUTS_LIST; i++)
    for (int j = 0; j < 6; j++)
      device_list[i].mac[j] = 0;

  //bluetooth_init(bluetooth_callback_handler);
  //int result = bluetooth_test_connection(4);
  //assert (result == 1, "Could not test the connection");
  //result = bluetooth_set_as_master();
  //assert (result == 1, "Could not set master mode");
}

void init_display (void)
{

}

void init_storage (void)
{
  //fat16_init();
}

void init_downlink (void)
{
  downlink_discover();
}

void slave_loop (void)
{
  while (state == SLAVE)
    {

    }
}

void master_loop (void)
{
  while (state == MASTER)
    {

    }
}

int main (void)
{
  init_bluetooth ();
  init_display ();
  init_storage ();
  init_downlink ();

  state = MASTER;

  while (true)
    {
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

