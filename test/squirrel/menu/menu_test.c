#ifndef X86

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE      9600ul

#else

#include <stdbool.h>
#include <SDL.h> // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#define _delay_ms SDL_Delay

#endif

#include <display/display.h>
#include <gui/gui_draw.h>
#include <gui/gui.h>

#include <gui/gui_data.h>
#include <squirrel.h>


#include <string.h>

#include <platform/buttons.h>

#define USART USARTC0

void menu_devices(void);
void menu_device_extension(void);

uint8_t menu_selected_device = -1;

uint8_t menu_devices_count = 0;
uint8_t menu_extensions_count = 0;

#ifdef X86
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
	for (i;i<EXTENSIONS_LIST;i++)
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

int main (
#ifndef X86
void
#else
int argc, char *argv[]
#endif
)
{

#ifndef X86
	OSC.CTRL = OSC_RC32MEN_bm;						// Internen 32Mhz Oszillator einschalten
	while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);	// Warten bis Oszillator stabil ist
	CCP = CCP_IOREG_gc;								// System Clock selection
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;				// DFLL ein (Auto Kalibrierung)

	set_bit(PORTC.DIR,4);
	clear_bit(PORTC.OUT,4);
#endif

	display_init();		// Initialize display

	button_init();

#ifdef X86
	createFakeDevices();
#endif

	display_gui_bootup_screen();
	menu_devices();

	//display_draw_image(0,0,(uint8_t*)tum_logo_f2);

	display_flip();
	while(true) 
	{
		display_gui_keypress(button_pressed());
		display_gui_refresh();	// Refresh gui drawings
		display_flip();
		_delay_ms(1);
	}
}
