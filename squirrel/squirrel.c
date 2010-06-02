/*
 * squirrel.c
 *
 */
#include "squirrel.h"
#include <stdlib.h>
void downlink_create_device_info_entry (uint8_t *address) {

	for (int k = 0; k < 16; k++) {
		if (device_list[k] == NULL) {
			// We haven't found the MAC, time to create a new entry
			device_list[k] = malloc (sizeof (device_info));
			device_list[k]->mac = *address;
			device_list[k]->class = downlink_get_class();

			for (int j = 0; j < 16; j++) {
				device_list[k]->sensor_types[j] = downlink_get_sensor_class(j);
				device_list[k]->actuator_types[j] = downlink_get_actuator_class(j + 0x80); // Actuators begin at ID 0x80
				// TODO config_types
			}

			return;
		} else if (device_list[k]->mac == found_mac) {
			// Already there, nothing to do
			return;
		}
	}

}

void downlink_discover(void) {
	uint8_t *found = bluetooth_cmd_search_devices();

	assert (found != NULL, "Malformed search result");

	int count = found[0];

	for (int i = 0; i < count; i++) {
		// Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
		squirrel_create_device_info_entry (found[1 + i * (16 + 6) + 16]);
	}

}

void bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {
		// Data package
		if (data_length == DOWNLINK_PACKAGE_LENGTH) {
			downlink_handle_package(data_package);
		} else if (data_length == UPLINK_PACKAGE_LENGTH) {
			uplink_handle_package(data_package);
		}
	} else if (callback_type == 1) {
		// Connect
	} else if (callback_type == 2) {
		// Disconnect
	}
}

void init_bluetooth (void) {
	bluetooth_init(bluetooth_callback_handler);
	int result = bluetooth_test_connection(4);
	assert (result == 1, "Could not test the connection");
	int result = bluetooth_set_as_master();
	assert (result == 1, "Could not set master mode");
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

	state = Master;

	while (true) {
		if (state == MASTER) {
			master_loop ();
		}

		if (state == SLAVE) {
			slave_loop ();
		}
	}
}

