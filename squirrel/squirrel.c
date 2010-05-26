/*
 * squirrel.c
 *
 */
#include "squirrel.h"

uint8_t state;

enum states {
	SLAVE, MASTER
};

struct device_info *device_list[16];

void downlink_create_device_info_entry (uint8_t *address[6]) {

	for (int k = 0; k < 16; k++) {
		if (device_list[k] == NULL) {
			// We haven't found the MAC, time to create a new entry
			device_list[k] = malloc (sizeof (device_info));
			device_list[k]->mac = found_mac;
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

	assert (found != NULL);

	int count = found[0];

	for (int i = 0; i < count; i++) {
		// Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
		squirrel_create_device_info_entry (found[1 + i * (16 + 6) + 16]);
	}

}

void init_bluetooth (void) {
	int result = bluetooth_test_connection(4);
	assert (result == 1);
	int result = bluetooth_set_as_master();
	assert (result == 1);
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

