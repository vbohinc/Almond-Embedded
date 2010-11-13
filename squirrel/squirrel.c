/*
 * squirrel.c
 *
 */

#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "squirrel.h"
#include "../drivers/bluetooth/bluetooth.h"
#include "../drivers/display/display.h"
#include "../drivers/platform/buttons.h"
//#include "../drivers/storage/storage.h"
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
#define POLL_INTERVALL 5

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
  for (uint8_t i = 0; i < 2; i++)
    {
      debug_pgm (PSTR("MAC: "));
      byte_to_hex (device_list[i].mac[0]);
      byte_to_hex (device_list[i].mac[1]);
      byte_to_hex (device_list[i].mac[2]);
      byte_to_hex (device_list[i].mac[3]);
      byte_to_hex (device_list[i].mac[4]);
      byte_to_hex (device_list[i].mac[5]);
      error_putc ('\n');
      debug_pgm (PSTR("CLASS: "));
      byte_to_hex (device_list[i].class);
      error_putc ('\n');
      debug_pgm (PSTR("VALUES: "));  
      for (uint8_t j = 0; j < EXTENSIONS_LIST; j++) 
        {
          byte_to_hex (device_list[i].extension_types[j]);
          error_putc (' ');
          error_putc ('-');
          error_putc ('>');
          error_putc (' ');
          byte_to_hex ((uint8_t) (device_list[i].values_cache[j] >> 8));
          byte_to_hex ((uint8_t) device_list[i].values_cache[j]);
          error_putc ('\n');
        }  
    }

}

#define valid(num) (num < NUTS_LIST && (device_list[num].mac[0] != 0 || device_list[num].mac[1] != 0 || device_list[num].mac[2] != 0 || device_list[num].mac[3] != 0 || device_list[num].mac[4] != 0 || device_list[num].mac[5] != 0))

bool bt_cmp (const char *add1, const char *add2)
{
  uint8_t i;
  for (i = 0; i < 6 && add1[i] == add2[i]; i++);
  return (i == 6);
}

bool squirrel_list (uint8_t num, uplink_payload_list *p)
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

bool squirrel_log (uplink_package *p) 
{
  uplink_payload_log *log = &(p->payload.log);

  for (uint8_t i = 0; i < 9; i++)
    {
      log->entries[i].time = i;
      log->entries[i].value = 42;
    }

  return true;
}

static void update_id (uint8_t num) 
{
  if (!valid (num)) return;

  bool err = false;
  
  device_list[num].class = downlink_get_nut_class (&err);

  for (uint8_t j = 0; j < EXTENSIONS_LIST && !err; j++)
    device_list[num].extension_types[j] = downlink_get_extension_class(j, &err);
}

static void update_values (uint8_t num) 
{
  if (!valid (num)) return;
  
  bool err = false;
  
  for (uint8_t i = 0; i < EXTENSIONS_LIST && !err; i++)
    {
      if (device_list[num].extension_types[i] < GENERIC_ACTOR)
        {
          device_list[num].values_cache[i] = downlink_get_sensor_value (i, &err);
          // log??
        }
    }
}

static void update_device_entry (const char *address)
{
  bool err;

  for (uint8_t k = 0; k < NUTS_LIST; k++)
    {
      if (!valid (k))
        memcpy (&device_list[k], (void *) address, 6);
      else if (!bt_cmp (device_list[k].mac, address))
        break;
        
      if (!bt_connect (device_list[k].mac) && downlink_is_nut (&err)) 
        {
           error_pgm (PSTR("Connection couldn't be established"));
           return;
        }
          
      update_id (k);
      update_values (k);
      downlink_bye (POLL_INTERVALL, &err);
           
      if (!bt_disconnect()) 
        {
          error_pgm(PSTR("Connection couldn't be closed"));
          // Hard reset module!
        }
       
      return;
    }

  error_pgm (PSTR("Out of Memory"));
}

void downlink_update(void)
{
  char result[8][6];

  // FIXME: UI
  if (bt_discover (result, NULL))
    for (uint8_t i = 0; i < 8; i++)
      update_device_entry (result[i]);
}

/* -----------------------------------------------------------------------
 * Init
 * ----------------------------------------------------------------------- */

int main (void)
{
  /* Internen 32Mhz Oszillator einschalten */
  //OSC.CTRL = OSC_RC32MEN_bm;

  /*external clock konfigurieren 9-12mhz*/
  OSC.XOSCCTRL = OSC_FRQRANGE1_bm | OSC_XOSCSEL3_bm | OSC_XOSCSEL1_bm | OSC_XOSCSEL0_bm;

  OSC.CTRL = OSC_XOSCEN_bm;
  while ((OSC.STATUS & OSC_XOSCRDY_bm) == 0);

  OSC.PLLCTRL = OSC_PLLSRC0_bm|OSC_PLLSRC1_bm|OSC_PLLFAC1_bm;

  /*external clock aktivieren */

  OSC.CTRL = OSC_XOSCEN_bm | OSC_PLLEN_bm;

  /* Warten bis Oszillator/PLL stabil ist */
  while ((OSC.STATUS & (OSC_PLLRDY_bm)) == 0);

  /* System Clock selection */
  CCP = CCP_IOREG_gc;
  CLK.CTRL = CLK_SCLKSEL_PLL_gc;

  /* DFLL ein (Auto Kalibrierung) */
  //DFLLRC32M.CTRL = DFLL_ENABLE_bm;

/* Behold the MAGIC Clock! */
    set_bit(PORTC.DIR,4);
    clear_bit(PORTC.OUT,4);


  error_init ();
  sei ();

  //debug_pgm(PSTR("Display Init"));          
  display_init ();
  
  debug_pgm(PSTR("INIT"));
  //debug_pgm(PSTR("Bluetooth Init"));          
  for (uint8_t i = 0; i < NUTS_LIST; i++)
    for (uint8_t j = 0; j < 6; j++)
      device_list[i].mac[j] = 0;
  bt_init();
  squirrel_state_set (MASTER);
            
  while (true)
    {
      display_flip ();
      if (state == MASTER)
        {
  		  debug_pgm(PSTR("fubar"));
          assert (bt_set_mode (BLUETOOTH_MASTER), "Could not set master mode");
		  _delay_ms(2000);
          downlink_update ();
  		  debug_pgm(PSTR("whoa?"));
          dump ();
          assert (bt_set_mode (BLUETOOTH_SLAVE), "Could not set slave mode");
          squirrel_state_set (SLAVE);
        }
      else if (state == SLAVE || state == SLAVE_BUSY)
        {
          // We wait for connections from the backend...
          uint8_t data[UPLINK_PACKAGE_LENGTH];
          uint8_t length = UPLINK_PACKAGE_LENGTH;
      
          if (bt_receive (data, &length, 0))
            uplink_process_pkg (data, length);
            
          //if (state == SLAVE)
          //  squirrel_state_set (MASTER);
        }
    }
}

