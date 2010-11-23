#include "menu.h"

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
