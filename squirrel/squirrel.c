/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
      ___                         ___                       ___           ___           ___                   
     /  /\          ___          /__/\        ___          /  /\         /  /\         /  /\                  
    /  /:/_        /  /\         \  \:\      /  /\        /  /::\       /  /::\       /  /:/_                 
   /  /:/ /\      /  /::\         \  \:\    /  /:/       /  /:/\:\     /  /:/\:\     /  /:/ /\    ___     ___ 
  /  /:/ /::\    /  /:/\:\    ___  \  \:\  /__/::\      /  /:/~/:/    /  /:/~/:/    /  /:/ /:/_  /__/\   /  /\
 /__/:/ /:/\:\  /  /:/~/::\  /__/\  \__\:\ \__\/\:\__  /__/:/ /:/___ /__/:/ /:/___ /__/:/ /:/ /\ \  \:\ /  /:/
 \  \:\/:/~/:/ /__/:/ /:/\:\ \  \:\ /  /:/    \  \:\/\ \  \:\/:::::/ \  \:\/:::::/ \  \:\/:/ /:/  \  \:\  /:/ 
  \  \::/ /:/  \  \:\/:/__\/  \  \:\  /:/      \__\::/  \  \::/~~~~   \  \::/~~~~   \  \::/ /:/    \  \:\/:/  
   \__\/ /:/    \  \::/        \  \:\/:/       /__/:/    \  \:\        \  \:\        \  \:\/:/      \  \::/   
     /__/:/      \__\/          \  \::/        \__\/      \  \:\        \  \:\        \  \::/        \__\/    
     \__\/                       \__\/                     \__\/         \__\/         \__\/                  


/*
 * squirrel.c
 *
 */
#include "squirrel.h"
#include <stdlib.h>
#include <string.h>

#define NUTS_LIST 16

static device_info device_list [NUTS_LIST];  

void squirrel_create_device_info_entry (const uint8_t *address) {
  
  for (int k = 0; k < NUTS_LIST; k++) {
		if (device_list[k].mac[0] == 0) {
			// We haven't found the MAC, time to create a new entry
			memcpy (&device_list[k], (void *) address, 6);
			
			device_list[k].class = downlink_get_nut_class (NULL);

			for (int j = 0; j < 16; j++) {
				device_list[k].sensor_types[j] = downlink_get_sensor_class(j, NULL);
				device_list[k].actuator_types[j] = downlink_get_actuator_class(j + 0x80, NULL); // Actuators begin at ID 0x80
				// TODO config_types
			}
			return;
		} else if (memcmp (device_list[k].mac, (void *) address, 6)) {
			// Already there, nothing to do
			return;
		}
	}

  error ("Out of Memory");
}

void downlink_discover(void) {
	uint8_t *found = bluetooth_cmd_search_devices();

	assert (found != NULL, "Malformed search result");

	int count = found[0];

	for (int i = 0; i < count; i++) {
		// Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
		squirrel_create_device_info_entry (&found[1 + i * (16 + 6) + 16]);
	}

}

void bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {
		// Data package
		if (data_length == DOWNLINK_PACKAGE_LENGTH) {
			//downlink_handle_package(data_package);
		} else if (data_length == UPLINK_PACKAGE_LENGTH) {
		//	uplink_handle_package(data_package);
		}
	} else if (callback_type == 1) {
		// Connect
	} else if (callback_type == 2) {
		// Disconnect
	}
}

void init_bluetooth (void) {
  // Mem Clean
  for (int i = 0; i < NUTS_LIST; i++)
		for (int j = 0; j < 6; j++)
		  device_list[i].mac[j] = 0;
  
	//bluetooth_init(bluetooth_callback_handler);
	//int result = bluetooth_test_connection(4);
	//assert (result == 1, "Could not test the connection");
	//result = bluetooth_set_as_master();
	//assert (result == 1, "Could not set master mode");
}

void init_display (void) {

}

void init_storage (void) {

}

void init_downlink (void) {
	downlink_discover();
}

int main (void) {
	init_bluetooth ();
	init_display ();
	init_storage ();
	init_downlink ();

	state = MASTER;

	while (true) {
		if (state == MASTER) {
			master_loop ();
		}

		if (state == SLAVE) {
			slave_loop ();
		}
	}
}

