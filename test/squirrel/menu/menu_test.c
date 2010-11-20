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

#endif

#include <display/display.h>
#include <gui/gui_draw.h>
#include <gui/gui.h>

#include <gui/gui_data.h>
#include <squirrel/squirrel.h>


#include <string.h>

#ifndef X86
#include <platform/buttons.h>
#endif

#define USART USARTC0

void menu_devices(void);
void menu_device_extension(void);

uint8_t menu_selected_device = -1;

uint8_t menu_devices_count = 0;
uint8_t menu_extensions_count = 0;

#ifdef X86

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

void
menu_entity_selected(int8_t option)
{
	if (option == menu_extensions_count-1)
		menu_devices(); //Back
	else
		menu_device_extension();
}

static char menu_extension_options[EXTENSIONS_LIST+2][MENU_OPTION_LENGHT];

void
menu_device_extension(void)
{
	uint8_t i;
	for (i=0; i<EXTENSIONS_LIST; i++)
	{
		if (device_list[menu_selected_device].extension_types[i] == INVALID)
			break;
		switch (device_list[menu_selected_device].extension_types[i])
		{
			case GENERIC_SENSOR:
				sprintf(menu_extension_options[i],"Generic: %d", device_list[menu_selected_device].values_cache[i]); break;
			case TEMPERATURE:
				sprintf(menu_extension_options[i],"Temp: %d C", device_list[menu_selected_device].values_cache[i]); break;
			case PRESSURE:
				sprintf(menu_extension_options[i],"Press: %d hPa", device_list[menu_selected_device].values_cache[i]); break;
			case LIGHT:
				sprintf(menu_extension_options[i],"Light: %d lux", device_list[menu_selected_device].values_cache[i]); break;
			case HUMIDITY:
				sprintf(menu_extension_options[i],"Humi: %d %%", device_list[menu_selected_device].values_cache[i]); break;
			default:
				sprintf(menu_extension_options[i],"UNK: %d", device_list[menu_selected_device].values_cache[i]);
		}
	}
	sprintf(menu_extension_options[i],"MAC: ");
	memcpy(menu_extension_options[i]+5,device_list[menu_selected_device].mac,12);
	sprintf(menu_extension_options[i+1],"-- BACK --");
	menu_extensions_count = i+2;
display_gui_menu("Values", menu_extension_options[0], menu_extensions_count, 0, &menu_entity_selected);
}


void
menu_devices_selected(int8_t option)
{
	
	if (option < 0)
	{
		menu_devices();
		return;
	}
	menu_selected_device = option;
	menu_device_extension();
}


static char menu_device_options[NUTS_LIST+1][MENU_OPTION_LENGHT];

void 
menu_devices(void)
{
	// Create menu for all found devices
	menu_selected_device = -1;
	uint8_t i;
	for (i=0; i<NUTS_LIST; i++)
	{
		if (valid(i))
		{
			switch (device_list[i].class)
			{
				case GENERIC_CLASS:
					sprintf(menu_device_options[i],"Generic"); break;
				case WEATHERSTATION:
					sprintf(menu_device_options[i],"Weatherstation"); break;
				default:
				sprintf(menu_device_options[i],"UNKNOWN");
			}
		} else {
			break;
		}
		
	}
	if (i==0)
	{
		sprintf(menu_device_options[i],"FOUND SHIT");
	}
	
	//sprintf(menu_device_options[i+1],"-- BACK --");
	//menu_devices_count = i+1;
	menu_devices_count = i;
	display_gui_menu("Pick a device", menu_device_options[0],menu_devices_count, 0, &menu_devices_selected);
}

#ifndef X86
int main (void)
#else
int main (int argc, char *argv[])
#endif
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

#ifndef X86
	button_init();
#endif

#ifdef X86
	createFakeDevices();
#endif

	display_gui_bootup_screen();
	menu_devices();

	//display_draw_image(0,0,(uint8_t*)tum_logo_f2);

	display_flip();
	while(true) 
	{
#ifdef X86
		uint8_t button = display_gui_key_none;
		SDL_Event keyevent;
		while (SDL_PollEvent(&keyevent)) {
			switch(keyevent.type){
				case SDL_KEYDOWN:
					// Handle key events
					printf("Key pressed: ");
     					switch(keyevent.key.keysym.sym){
						case SDLK_UP:
							printf("^");
							button = display_gui_key_up;
							break;
						case SDLK_DOWN:
							printf("v");
							button = display_gui_key_down;
							break;
						case SDLK_LEFT:
							printf("<");
							button = display_gui_key_left;
							break;
						case SDLK_RIGHT:
							printf(">");
							button = display_gui_key_right;
							break;
						case SDLK_a:
							printf("A");
							button = display_gui_key_a;
							break;
						case SDLK_s:
							printf("B");
							button = display_gui_key_b;
							break;
						case SDLK_x:
							printf("(X) SDL quit escape key");
							exit(0);
							break;
						default:
							break;
					}
					printf("\n");
					break;
				case SDL_QUIT:
					printf("Quitting...\n");
					exit(0);
					break;
			}
		}
#else
		uint8_t button = button_pressed();
#endif
		if (button != display_gui_key_none)	{
			display_gui_keypress(button);
		}
		
		display_gui_refresh();	// Refresh gui drawings
		display_flip();
#ifndef X86
		_delay_ms(1);
#else
		SDL_Delay(100);
#endif
	}
}
