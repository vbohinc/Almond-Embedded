/*
 * squirrel.c
 *
 */

#include <util/delay.h>
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
  char mac[6];
  uint8_t class;
  uint8_t extension_types[EXTENSIONS_LIST];
  uint16_t values_cache[EXTENSIONS_LIST];
};

static device_info device_list [NUTS_LIST];

static void dump (void) 
{
  for (uint8_t i = 0; i < 3; i++)
    {
      debug_pgm (PSTR("\nMAC: "));
      byte_to_hex (device_list[i].mac[0]);
      byte_to_hex (device_list[i].mac[1]);
      byte_to_hex (device_list[i].mac[2]);
      byte_to_hex (device_list[i].mac[3]);
      byte_to_hex (device_list[i].mac[4]);
      byte_to_hex (device_list[i].mac[5]);
      debug_pgm (PSTR("\nCLASS: "));
      byte_to_hex (device_list[i].class);
      debug_pgm (PSTR("\nEXTENSIONS: "));
      for (uint8_t j = 0; j < EXTENSIONS_LIST; j++)
        byte_to_hex (device_list[i].extension_types[j]);
        
      //for (uint8_t j = 0; j < EXTENSIONS_LIST; j++)
      //  byte_to_hex (device_list[i].values_cache[j]);  
    }

}

#define valid(num) (num < NUTS_LIST && (device_list[num].mac[0] != 0 || device_list[num].mac[1] != 0 || device_list[num].mac[2] != 0 || device_list[num].mac[3] != 0 || device_list[num].mac[4] != 0 || device_list[num].mac[5] != 0))

extern bool squirrel_list (uint8_t num, uplink_payload_list *p)
{
  if (valid (num))
    {
      for (uint8_t i = 0; i < 6; i++)
        p->bt_address[i] = device_list[num].mac[i];

      p->nut_class = device_list[num].class;

      for (uint8_t i = 0; i < EXTENSIONS_LIST; i++)
        p->extension_class[i] = device_list[num].extension_types[i];

      return true;
    }
  else
    {
      return false;
    }
}

void update_id (uint8_t num) 
{
  if (!valid (num)) return;
  
  device_list[num].class = downlink_get_nut_class (NULL);

  for (uint8_t j = 0; j < 6; j++)
    device_list[num].extension_types[j] = downlink_get_extension_class(j, NULL);
}

void update_values (uint8_t num) 
{
  if (!valid (num)) return;
  
  for (uint8_t i = 0; i < 6; i++)
    {
      if (device_list[num].extension_types[i] < GENERIC_ACTOR)
        {
          device_list[num].values_cache[i] = downlink_get_sensor_value (i, NULL);
          // log??
        }
    }
}


void squirrel_create_device_info_entry (const uint8_t *address)
{
  // ??? debug bluetooth address
  for (uint8_t k = 0; k < NUTS_LIST; k++)
    {
      if (!valid (k))
        {
          // We haven't found the MAC, time to create a new entry
          memcpy (&device_list[k], (void *) address, 6);
          
          switch (bluetooth_connect (device_list[k].mac)) {
            case 0:
              debug_pgm(PSTR("connected"));
              bluetooth_is_connected = 1;
              break;
              
            case 1:
              debug_pgm(PSTR("1"));
              return;
            
            case 2:
              debug_pgm(PSTR("2"));
              return;
            
            case 3:
              debug_pgm(PSTR("3"));
              return;
                          
            default:
              debug_pgm(PSTR("FUCK"));
              return;
          }

          _delay_ms (1000);
          update_id (k);
          update_values (k);
            
          if (bluetooth_disconnect(2) != 1) {
            debug_pgm(PSTR("could not disconnect"));
          } else {
            debug_pgm(PSTR("disconnect"));
            bluetooth_is_connected = 0;
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

void downlink_update(void)
{
  char *found = bluetooth_cmd_search_devices_debug();

  assert (found != NULL, "Malformed search result");

  uint8_t count = found[0];
  
  debug_pgm(PSTR("Num Found:"));
  error_putc(count+48);
  error_putc(13);

  char arr[20];

  for (uint8_t i = 0; i < count; i++)
    {
      debug_pgm(PSTR("Device found"));
      
      bluetooth_array_to_address(found + 1 + i * (16 + 6) + 16, arr, 1);
      
      debug(arr);
         
      // Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
      squirrel_create_device_info_entry (&found[1 + i * (16 + 6) + 16]);
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
    debug_pgm(PSTR("Test connection")); 
  uint8_t result = bluetooth_test_connection(4);
  assert (result == 1, "Could not test the connection");
}

void init_display (void)
{

}

void init_storage (void)
{
  //fat16_init (0);
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
  /* Behold the MAGIC Clock! */

  /* Internen 32Mhz Oszillator einschalten */
  OSC.CTRL = OSC_RC32MEN_bm;

  /* Warten bis Oszillator stabil ist */
  while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);

  /* System Clock selection */
  CCP = CCP_IOREG_gc;
  CLK.CTRL = CLK_SCLKSEL_RC32M_gc;

  /* DFLL ein (Auto Kalibrierung) */
  DFLLRC32M.CTRL = DFLL_ENABLE_bm;

  error_init ();
  sei ();

  debug_pgm(PSTR("Bluetooth Init"));          
  init_bluetooth ();
  
  debug_pgm(PSTR("Display Init"));          
  init_display ();
  
  debug_pgm(PSTR("Storage Init"));          
  init_storage ();
  
  squirrel_state_set (MASTER);
  debug_pgm(PSTR("Mainloop"));
  
  dump ();
            
  while (true)
    {
      // Check for input events...
      // redraw UI
      // TIMER events!

      // if (something changed)
      draw_ui (0, 0);

      if (state == MASTER)
        {
          assert (bluetooth_set_as_master() == 1, "Could not set master mode");
          downlink_update ();
          dump ();
          assert (bluetooth_set_as_slave() == 1, "Could not set slave mode");
          squirrel_state_set (SLAVE);
        }
      else if (state == SLAVE)
        {
          // We wait for connections from the backend...
          bluetooth_process_data ();
          squirrel_state_set (MASTER);
        }
    }
}

