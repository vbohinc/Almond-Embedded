

#include <stdint.h>
#include <display/display.h>
#include <gui/gui.h>
#include <squirrel.h>
#include <gui/gui_draw.h>
#include <string.h>
#include <gui/menu.h>

#include <platform/buttons.h>

#include <stdlib.h>
#include <string.h>
#include <downlink/downlink.h>
#include <uplink/uplink.h>
#include <package_types.h>

//---------------------------------------------------------------------------------
#ifndef X86

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE      9600ul

#include <bluetooth/bluetooth.h>



//---------------------------------------------------------------------------------
#else
//---------------------------------------------------------------------------------

#include <stdbool.h>
#include <SDL.h> // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#define _delay_ms SDL_Delay

void bt_init(void (*upate_percentage) (uint16_t))
{
	//init fifo & uart
    _delay_ms (200);
    upate_percentage(10);
	//empty fifo
    _delay_ms (200);
    upate_percentage(20);
    _delay_ms (200);
    	//clear address
    upate_percentage(30);
    _delay_ms (200);
    	//set slave
    upate_percentage(40);
}

void downlink_update (void (*update_callback)(const uint16_t progress))
{
	//bt_discover blabla
    for (uint16_t i = 0; i < 300; i++)
    {
        if((i % 5) == 0)
            update_callback(40+i/5);
        //uart_receive();
        _delay_ms (1);
    }
	update_callback(100);
}

//Create fake devices, only on X86 needed

device_info device_list [NUTS_LIST];

void createFakeDevices()
{
	memcpy(device_list[0].mac,"0123456789AB",12);
	device_list[0].class = WEATHERSTATION;
	int i;
	for (i=0; i<4;i++)
	{
		device_list[0].extension_types[i]=GENERIC_ACTOR;
		device_list[0].values_cache[i]=i;
	}
	for (;i<EXTENSIONS_LIST;i++)
		device_list[0].extension_types[i] = INVALID;

	memcpy(device_list[1].mac,"BA9876543210",12);
	device_list[1].class = GENERIC_CLASS;
	for (i=0; i<5;i++)
	{
		device_list[1].extension_types[i]=4-i;
		device_list[1].values_cache[i]=4-i;
	}
	for (;i<EXTENSIONS_LIST;i++)
		device_list[1].extension_types[i] = INVALID;
}

uint8_t hex_to_char(uint8_t hex)
{
	if (hex<=9)
		return hex+'0';
	else if (hex>9 && hex <= 15)
		return hex+'A'-10;
	else
		return 0;
}

#endif
//---------------------------------------------------------------------------------



int main(int argc, char **argv)
{
    display_init ();
    display_gui_bootup_screen();
    bt_init(display_gui_bootup_update_callback);

	enum menu_return men_ret;

	uint8_t state = MASTER;

	while (true)
		{
			

		    if (state == MASTER)
		    {
		        assert (bt_set_mode (BLUETOOTH_MASTER), "Could not set master mode");

#ifdef X86
		        downlink_update (display_gui_bootup_update_callback);
#endif
		        debug_pgm (PSTR ("whoa?"));
		        assert (bt_set_mode (BLUETOOTH_SLAVE), "Could not set slave mode");
#ifdef X86
				createFakeDevices();
#endif
				state = SLAVE;		        
		    }

		    else
		        if (state == SLAVE || state == SLAVE_BUSY)
		        {
		            // We wait for connections from the backend...
		            men_ret = menu_update();
				_delay_ms(50); //fake Squirrel working time
				if (men_ret == MEN_NEW_SEARCH)
					state = MASTER;
		        }
		}
}
