/**
 * menu.c - Main and Sub-Menues
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 */

#include "menu.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <squirrel.h>
#include <gui/gui.h>
#include <platform/buttons.h>
#include <gui/pong.h>

enum menu_return menu_new_return = -1;

uint8_t menu_devices_count = 0;
uint8_t menu_extensions_count = 0;
uint8_t menu_selected_device = -1;

bool menu_initialized = false;

void menu_devices(void);
void menu_device_extension(void);
void menu_main(void);


void
menu_entity_selected(int8_t option)
{
	if (option == menu_extensions_count-1)
		menu_devices(); //Back
	else
		menu_device_extension();
}

void
menu_device_extension(void)
{
	static char menu_extension_options[EXTENSIONS_LIST+2][MENU_OPTION_LENGHT];
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
				sprintf(menu_extension_options[i],"Light: %d cd", device_list[menu_selected_device].values_cache[i]); break;
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
	if (option == menu_devices_count-1) //back
	{
		menu_main();
	}
	else if (option == menu_devices_count -2)  // Rescan
	{
		menu_new_return = MEN_NEW_SEARCH;
	}
	else
	{
		menu_selected_device = option;
		menu_device_extension();
	}
}

void 
menu_devices(void)
{
	static char menu_device_options[NUTS_LIST+1][MENU_OPTION_LENGHT];
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
	
	sprintf(menu_device_options[i++],"-- RESCAN --");
	sprintf(menu_device_options[i++],"-- BACK --");
	menu_devices_count = i;
	//menu_devices_count = i;
	display_gui_menu("Pick a device", menu_device_options[0],menu_devices_count, 0, &menu_devices_selected);
}

void
menu_main_selected(int8_t option)
{
	switch (option)
	{
		case 0: //Show Nuts
			menu_devices(); break;
		case 1: //Test image
			display_gui_image(testimg, &menu_main); break;
		case 2: //Pong game
			pong(&menu_main); break;
		case 3: //Credits
			display_gui_about(&menu_main); break;
		default:
			break;
	}
}

void
menu_main(void)
{
	// Main menu
	static char options[4][MENU_OPTION_LENGHT];
	sprintf(options[0],"Show Nuts");
	sprintf(options[1],"Test Image");
	sprintf(options[2],"Pong");
	sprintf(options[3],"Credits");
 	display_gui_menu("Main Menu", *options, 4,0, &menu_main_selected);
}


enum menu_return menu_update(void)
{
	if (menu_new_return == MEN_NEW_SEARCH) //comes back here after a rescan
	{
		menu_devices();	
	}
	menu_new_return = MEN_CONTINUE;

	if (!menu_initialized)
	{
		menu_initialized = true;
		menu_main();
	}
	
	enum display_gui_keys button = button_pressed();

	if (button != display_gui_key_none)	{
		display_gui_keypress(button);
	}
		
	
	display_gui_refresh();	// Refresh gui drawings
	display_flip();
	return menu_new_return;
}