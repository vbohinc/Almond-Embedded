/**
 * @file squirrel.c 
 * the Squirrel's c file
 */

/* Common */

#include <stdlib.h>
#include <string.h>
#include "squirrel.h"
#include "util.h"

/* Protocols */

#include <downlink/downlink.h>
#include <uplink/uplink.h>
#include <package_types.h>

/* Drivers */

#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <rtc/rtc.h>
#include <bluetooth/bluetooth.h>
#include <sd/sd.h>
#include <fat16/fat16.h>
#include <platform/buttons.h>

/* UI */

#include <gui/gui.h>
#include <gui/menu.h>

/* -----------------------------------------------------------------------
 * Squirrel State
 * ----------------------------------------------------------------------- */

static uint8_t state;

uint8_t squirrel_state_get(void)
{
	return state;
}

void squirrel_state_set(uint8_t s)
{
	state = s;
}

/* -----------------------------------------------------------------------
 * Uplink hooks
 * ----------------------------------------------------------------------- */

bool squirrel_list(uint8_t num, uplink_payload_list * p)
{
	if (!valid(num)) {
		return false;
	}

	for (uint8_t i = 0; i < 12; i++)
		p->bt_address[i] = device_list[num].mac[i];

	p->nut_class = device_list[num].class;

	for (uint8_t i = 0; i < EXTENSIONS_LIST; i++)
		p->extension_class[i] = device_list[num].extension_types[i];

	return true;
}

bool squirrel_log(uplink_package * p)
{

	uplink_payload_log *log = &(p->payload.log);
	return log_read(log->bt_address, log->extension_id, p->id,
			(uint8_t *) log->entries);

#if 0
	for (uint8_t i = 0; i < 8; i++) {
		log->entries[i].time = i + 1;
		log->entries[i].value = 42;
	}
	return true;
#endif
}

/* -----------------------------------------------------------------------
 * Nut Handling
 * ----------------------------------------------------------------------- */

#define POLL_INTERVALL 5

device_info device_list[NUTS_LIST];

static bool bt_cmp(const char *add1, const char *add2)
{
	uint8_t i;
	for (i = 0; i < 12 && add1[i] == add2[i]; i++) ;
	return (i == 12);
}

static void downlink_update_info(uint8_t num)
{
	bool err = false;
	uint8_t j;

	if (!valid(num))
		return;

	device_list[num].class = downlink_get_nut_class(&err);

	if (err)
		debug_pgm(PSTR("GET NUT_CLASS FAILED"));

	for (j = 0; j < EXTENSIONS_LIST && !err; j++)
		device_list[num].extension_types[j] =
		    downlink_get_extension_class(j, &err);

	for (; j < EXTENSIONS_LIST; j++)
		device_list[num].extension_types[j] = INVALID;
}

static void downlink_update_values(uint8_t num)
{
	bool err = false;

	if (!valid(num))
		return;

	for (uint8_t i = 0; i < EXTENSIONS_LIST; i++) {
		if (device_list[num].extension_types[i] < GENERIC_ACTOR) {
			device_list[num].values_cache[i] =
			    downlink_get_sensor_value(i, &err);
		}
	}
}

static void downlink_work (void) 
{
	bool err;

	for (uint8_t k = 0; k < NUTS_LIST && valid(k); k++) {
		
		if (!bt_set_mode (BLUETOOTH_MASTER)) {
			debug_pgm(PSTR("Couldn't set master!"));
			return;
		}

		if (!bt_connect(device_list[k].mac)) {
			debug_pgm(PSTR("Connection failed"));
			bt_disconnect();
			return;
		}

		if (!downlink_is_nut(&err)) {
			error_pgm(PSTR("Remote dev is not a nut'"));
			bt_disconnect();
			continue;
		}	

		downlink_update_info(k);
		downlink_update_values(k);
		downlink_bye(POLL_INTERVALL, &err);

		bt_disconnect();
	}
}

static void downlink_log (void)
{
	debug_pgm ("Writing log for devices");

	for (uint8_t k = 0; k < NUTS_LIST && valid(k); k++)
		for (uint8_t i = 0; i < EXTENSIONS_LIST; i++)
			if (device_list[k].extension_types[i] < GENERIC_ACTOR)
				log_write (device_list[k].mac, i, get_time(), device_list[k].values_cache[i]);

	debug_pgm ("Finished writing log for devices");
}


static void downlink_create (const char *address)
{
	uint8_t k = 0;
	bool err;

	for (; k < NUTS_LIST; k++) {
		if (bt_cmp(device_list[k].mac, address))
			return;
		else if (!valid(k))
			break;
	}

	if (k == NUTS_LIST) {
		debug_pgm(PSTR("Out of Memory"));
		bt_disconnect();
		return;
	}

	if (!bt_set_mode (BLUETOOTH_MASTER)) {
		debug_pgm(PSTR("Couldn't set master!"));
		return;
	}

	if (!bt_connect(device_list[k].mac)) {
		debug_pgm(PSTR("Conn failed"));
		bt_disconnect();
		return;
	}

	if (!downlink_is_nut(&err) && !err) {
		error_pgm(PSTR("Remote dev is not a nut'"));
		bt_disconnect();
		return;
	}	

	memcpy(&device_list[k], (void *)address, 12);

	downlink_update_info(k);
	downlink_update_values(k);
	downlink_bye(POLL_INTERVALL, &err);

	bt_disconnect();
}

void downlink_init(void)
{
	char result[8][12];

	if (!bt_set_mode (BLUETOOTH_MASTER)) {
		debug_pgm(PSTR("Couldn't set master!"));
		return;
	}

	for (uint8_t i = 0; i < 8; i++)
		for (uint8_t j = 0; j < 12; j++)
			result[i][j] = 0;

	if (bt_discover(result, display_gui_bootup_update_callback))
		for (uint8_t i = 0; i < 8; i++)
			downlink_create(result[i]);
	else
		debug_pgm(PSTR("Search failed"));
}



/* -----------------------------------------------------------------------
 * Debug
 * ----------------------------------------------------------------------- */

void createFakeDevices_address(void)
{
	char result[8][12];
	memcpy(result[0], "00126f095065", 12);

	downlink_create(result[0]);
}

void createFakeDevices_full(void)
{
	memcpy(device_list[0].mac, "00126f095065", 12);
	device_list[0].class = WEATHERSTATION;
	int i;
	for (i = 0; i < 5; i++) {
		device_list[0].extension_types[i] = i;
		device_list[0].values_cache[i] = i;
	}
	for (; i < EXTENSIONS_LIST; i++)
		device_list[0].extension_types[i] = INVALID;
}



/* -----------------------------------------------------------------------
 * Init
 * ----------------------------------------------------------------------- */

int main(void)
{
	enum menu_return men_ret;

	/* Bluetooth reset */
	set_bit(PORTC.DIR, 4);
	//set_bit (PORTC.OUT, 4);
	//_delay_ms (1000);
	clear_bit(PORTC.OUT, 4);

	OSC.CTRL = OSC_RC32MEN_bm;	// Internen 32Mhz Oszillator einschalten
	while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0) ;	// Warten bis Oszillator stabil ist
	CCP = CCP_IOREG_gc;	// System Clock selection
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	display_init();
	button_init();
	display_gui_bootup_screen();
	//Set Prescaler to devide by 4
	CCP = CCP_IOREG_gc;
	CLK.PSCTRL = CLK_PSBCDIV_2_2_gc;

	/*external clock konfigurieren 9-12mhz */
	OSC.XOSCCTRL =
	    OSC_FRQRANGE1_bm | OSC_XOSCSEL3_bm | OSC_XOSCSEL1_bm |
	    OSC_XOSCSEL0_bm;

	/*external clock aktivieren */
	OSC.CTRL = OSC_XOSCEN_bm;

	/* PLL auf 10x stellen */
	OSC.PLLCTRL =
	    OSC_PLLSRC0_bm | OSC_PLLSRC1_bm | OSC_PLLFAC3_bm | OSC_PLLFAC1_bm;

	/*auf stabile clock warten */

	while ((OSC.STATUS & OSC_XOSCRDY_bm) == 0) ;
	display_gui_bootup_update_callback(5);

	/*external clock + PLL aktivieren */
	OSC.CTRL = OSC_XOSCEN_bm | OSC_PLLEN_bm;

	/* Warten bis PLL stabil ist */
	while ((OSC.STATUS & (OSC_PLLRDY_bm)) == 0) ;
	display_gui_bootup_update_callback(10);
	/* System Clock selection */
	CCP = CCP_IOREG_gc;

	CLK.CTRL = CLK_SCLKSEL_PLL_gc;

	display_gui_bootup_update_callback(15);
	rtc_init();
	sei();
	set_time (1337);
#if 0
	uint8_t time;


	while (true) {
		if (time != get_time ()) {
			time = get_time ();
			debug_pgm (PSTR ("Tock!"));
		}
	}
#endif

	bt_init(display_gui_bootup_update_callback);	
	downlink_init();

	sd_init();
	
  if (fat16_init(0) != 1) {
	    debug_pgm(PSTR("FAT16 init fail!"));
      _delay_ms (1000);
  }
	
#if 0
  debug_pgm(PSTR("Fake devices:"));
	display_gui_bootup_update_callback(0);
	createFakeDevices_address();
	isplay_gui_bootup_update_callback(100);
#endif

	squirrel_state_set(MASTER);

	while (true) {

		if (state == MASTER) {
			display_gui_bootup_update_callback(0);
			downlink_work ();
			display_gui_bootup_update_callback(50);
			downlink_log ();
			display_gui_bootup_update_callback(100);

			bt_disconnect ();
			squirrel_state_set(SLAVE);
		} else if (state == SLAVE || state == SLAVE_BUSY) {
			uint8_t data[UPLINK_PACKAGE_LENGTH];

			// We wait for connections from the backend...    
			if (bt_receive(data, UPLINK_PACKAGE_LENGTH, 0))
				uplink_process_pkg(data);
		}

#ifdef DBUTTONS_REMOTE
		enum display_gui_keys buttonlist[] =
		    { display_gui_key_a, display_gui_key_a, display_gui_key_a };
		for (uint8_t i = 0; i < 3; i++) {
			set_gui_key_pressed(buttonlist[i]);
			menu_update();
		}
#endif				//DBUTTONS_REMOTE

		men_ret = menu_update();

		if (men_ret == MEN_NEW_SEARCH)
			squirrel_state_set(MASTER);
	}

}
