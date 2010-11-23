
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
#include <stdint.h>

#define _delay_ms SDL_Delay

void bt_init(void (*upate_percentage) (uint16_t))
{
	//init fifo & uart
    _delay_ms (500);
    upate_percentage(10);
	//empty fifo
    _delay_ms (500);
    upate_percentage(20);
    _delay_ms (500);
    	//clear address
    upate_percentage(30);
    _delay_ms (500);
    	//set slave
    upate_percentage(40);
}

void downlink_update (void (*update_callback)(const uint16_t progress))
{
	//bt_discover blabla
    for (uint16_t i = 0; i < 600; i++)
    {
        if((i % 10) == 0)
            update_callback(40+i/10);
        //uart_receive();
        _delay_ms (1);
    }
	update_callback(100);
}

volatile uint8_t state;

//Create fake devices, only on X86 needed

device_info device_list [NUTS_LIST];

void createFakeDevices()
{
	memcpy(device_list[0].mac,"0123456789AB",12);
	int i;
	for (i=0; i<5;i++)
	{
		device_list[0].extension_types[i]=i;
		device_list[0].values_cache[i]=i;
	}
	for (;i<EXTENSIONS_LIST;i++)
		device_list[0].extension_types[i] = INVALID;
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

	state = MASTER;

	while (true)
		{
		    display_flip ();

		    if (state == MASTER)
		    {
		        assert (bt_set_mode (BLUETOOTH_MASTER), "Could not set master mode");
		        downlink_update (display_gui_bootup_update_callback);
		        debug_pgm (PSTR ("whoa?"));
		        assert (bt_set_mode (BLUETOOTH_SLAVE), "Could not set slave mode");
				createFakeDevices();
				state = SLAVE;		        
		    }

		    else
		        if (state == SLAVE || state == SLAVE_BUSY)
		        {
		            // We wait for connections from the backend...
		            

		        }
		}
}
