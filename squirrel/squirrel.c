/**
 * squirrel.c - the Squirrel
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "squirrel.h"
#include <bluetooth/bluetooth.h>
#include <gui/gui.h>
#include <platform/buttons.h>
#include <downlink/downlink.h>
#include <uplink/uplink.h>
#include <package_types.h>
#include <gui/menu.h>

/* -----------------------------------------------------------------------
 * Squirrel State
 * ----------------------------------------------------------------------- */

static uint8_t state;

uint8_t squirrel_state_get (void)
{
    return state;
}

void squirrel_state_set (uint8_t s)
{
    state = s;
}

/* -----------------------------------------------------------------------
 * Uplink hooks
 * ----------------------------------------------------------------------- */

bool squirrel_list (uint8_t num, uplink_payload_list *p)
{
  if (!valid (num))
	{
	    return false;
	}

  for (uint8_t i = 0; i < 12; i++)
    p->bt_address[i] = device_list[num].mac[i];

  p->nut_class = device_list[num].class;

  for (uint8_t i = 0; i < EXTENSIONS_LIST; i++)
    p->extension_class[i] = device_list[num].extension_types[i];

  return true;
}

bool squirrel_log (uplink_package *p)
{
	if (p->id != 0)
		return false;

  uplink_payload_log *log = & (p->payload.log);


  for (uint8_t i = 0; i < 8; i++)
    {
      log->entries[i].time = i+1;
      log->entries[i].value = 42;
    }

  return true;
}

/* -----------------------------------------------------------------------
 * Nut Handling
 * ----------------------------------------------------------------------- */

#define POLL_INTERVALL 5

device_info device_list [NUTS_LIST];

bool bt_cmp (const char *add1, const char *add2)
{
    uint8_t i;
    for (i = 0; i < 12 && add1[i] == add2[i]; i++);
    return (i == 12);
}

static void dump (void)
{
  for (uint8_t i = 0; i < 1; i++)
    {
      debug_pgm (PSTR ("MAC: "));
      for (uint8_t j = 0; j < 12; j++)
        error_putc (device_list[i].mac[j]);
      error_putc ('\n');
      
      debug_pgm (PSTR ("CLASS: "));
      byte_to_hex (device_list[i].class);
      error_putc ('\n');
      
      debug_pgm (PSTR ("VALUES: "));

      for (uint8_t j = 0; j < 6; j++)
        {
          byte_to_hex (device_list[i].extension_types[j]);
          error_putc ('-');
          error_putc ('>');
          error_putc (' ');
          byte_to_hex ( (uint8_t) (device_list[i].values_cache[j] >> 8));
          byte_to_hex ( (uint8_t) device_list[i].values_cache[j]);
          error_putc ('\n');
        }
    }
}

static void update_info (uint8_t num)
{
  bool err = false;
  uint8_t j;

  if (!valid (num))
    return;
  
  device_list[num].class = downlink_get_nut_class (&err);

  if (err)
    debug_pgm (PSTR ("GET NUT_CLASS FAILED"));
    
  for (j = 0; j < EXTENSIONS_LIST && !err; j++)
    device_list[num].extension_types[j] = downlink_get_extension_class (j, &err);

  for (; j < EXTENSIONS_LIST; j++)
    device_list[num].extension_types[j] = INVALID;
}

static void update_values (uint8_t num)
{
  bool err = false;

  if (!valid (num))
    return;

  for (uint8_t i = 0; i < EXTENSIONS_LIST; i++)
    {
      if (device_list[num].extension_types[i] < GENERIC_ACTOR)
        {
          device_list[num].values_cache[i] = downlink_get_sensor_value (i, &err);
          // log??
        }
    }
}

static void create_device_entry (const char *address)
{
  bool err;

  for (uint8_t k = 0; k < NUTS_LIST; k++)
    {
      if (bt_cmp (device_list[k].mac, address))
        return;
      else
        if (valid (k))
          continue;

      for (uint8_t i = 0; i < 3 && !bt_connect (address); i++)
        {
          error_pgm (PSTR ("Conn failed"));
          bt_disconnect ();
        }

      if (!downlink_is_nut (&err))
        {
          error_pgm (PSTR ("No Nut today"));
          return;
        }

      memcpy (&device_list[k], (void *) address, 12);

      update_info (k);
      update_values (k);
      downlink_bye (POLL_INTERVALL, &err);

      if (!bt_disconnect())
        error_pgm (PSTR ("Conn not closed"));

      return;
    }

  error_pgm (PSTR ("Out of Memory"));
}

//00126f095065

void createFakeDevices_address()
{
	char result[8][12];
	memcpy(result[0],"00126f095065",12);
	
      create_device_entry (result[0]);
}

void createFakeDevices_full()
{
	memcpy(device_list[0].mac,"00126f095065",12);
	device_list[0].class = WEATHERSTATION;
	int i;
	for (i=0; i<5;i++)
	{
		device_list[0].extension_types[i]=i;
		device_list[0].values_cache[i]=i;
	}
	for (;i<EXTENSIONS_LIST;i++)
		device_list[0].extension_types[i] = INVALID;
}

void downlink_update (void)
{
  char result[8][12];

  for (uint8_t i = 0; i < 8; i++)
    for (uint8_t j = 0; j < 12; j++)
      result[i][j] = 0;

  if (bt_discover (result, display_gui_bootup_update_callback))
    for (uint8_t i = 0; i < 8; i++)
      create_device_entry (result[i]);
  else
    debug_pgm (PSTR ("Search failed"));
}

/* -----------------------------------------------------------------------
 * Init
 * ----------------------------------------------------------------------- */

int main (void)
{
    /* Bluetooth einschalten */
    set_bit (PORTC.DIR, 4);
    clear_bit (PORTC.OUT, 4);
 
    OSC.CTRL = OSC_RC32MEN_bm;                      // Internen 32Mhz Oszillator einschalten
    while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);    // Warten bis Oszillator stabil ist
    CCP = CCP_IOREG_gc;                             // System Clock selection
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
    display_init ();
	button_init();
    display_gui_bootup_screen();
    //Set Prescaler to devide by 4
    CCP = CCP_IOREG_gc;
    CLK.PSCTRL = CLK_PSBCDIV_2_2_gc;

    /*external clock konfigurieren 9-12mhz*/
    OSC.XOSCCTRL = OSC_FRQRANGE1_bm | OSC_XOSCSEL3_bm | OSC_XOSCSEL1_bm | OSC_XOSCSEL0_bm;

    /*external clock aktivieren */
    OSC.CTRL = OSC_XOSCEN_bm;

    /* PLL auf 10x stellen */
    OSC.PLLCTRL = OSC_PLLSRC0_bm | OSC_PLLSRC1_bm | OSC_PLLFAC3_bm | OSC_PLLFAC1_bm;

    /*auf stabile clock warten */

    while ((OSC.STATUS & OSC_XOSCRDY_bm) == 0);
    display_gui_bootup_update_callback(5);

    /*external clock + PLL aktivieren */
    OSC.CTRL = OSC_XOSCEN_bm | OSC_PLLEN_bm;

    /* Warten bis PLL stabil ist */
    while ((OSC.STATUS & (OSC_PLLRDY_bm)) == 0);
    display_gui_bootup_update_callback(10);
    /* System Clock selection */
    CCP = CCP_IOREG_gc;

    CLK.CTRL = CLK_SCLKSEL_PLL_gc;
    
    display_gui_bootup_update_callback(15);
   
	sei ();
    bt_init(display_gui_bootup_update_callback);
    squirrel_state_set (MASTER);
    
	enum menu_return men_ret;

    while (true)
    {
        display_flip ();

        if (state == MASTER)
        {
			display_gui_bootup_update_callback(20);
            assert (bt_set_mode (BLUETOOTH_MASTER), "Could not set master mode");
            downlink_update ();
		

///debug_pgm(PSTR("Fake devices:"));
/*display_gui_bootup_update_callback(0);
		createFakeDevices_full();
display_gui_bootup_update_callback(100);*/
            

//dump ();

           // _delay_ms (500);
           // bt_connect (device_list[0].mac);
           // bool err;
           // downlink_set_actuator_value (4, 1, &err);
		//led_on = !led_on;
            //downlink_bye (10, &err);
            //bt_disconnect ();

            assert (bt_set_mode (BLUETOOTH_SLAVE), "Could not set slave mode");
            squirrel_state_set (SLAVE);
            bt_set_mode (BLUETOOTH_SLAVE);
        }

        else
            if (state == SLAVE || state == SLAVE_BUSY)
            {

                // We wait for connections from the backend...
                uint8_t data[UPLINK_PACKAGE_LENGTH];
              
                if (bt_receive (data, UPLINK_PACKAGE_LENGTH, 0))
		{
                    uplink_process_pkg (data, &menu_slave_connected);
		}

		men_ret = menu_update();
		if (men_ret == MEN_NEW_SEARCH)
			 squirrel_state_set (MASTER);


                //if (state == SLAVE)
                //  squirrel_state_set (MASTER);
            }
    }
}

