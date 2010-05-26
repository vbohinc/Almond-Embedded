/*
 * squirrel.c
 *
 */
#include "squirrel.h"

enum state {
	SLAVE, MASTER
};

struct device_info *device_list[16];

void squirrel_create_device_info_entry (uint8_t *address[6]) {

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

void master_discover(void) {

	int result = bluetooth_set_as_master();
	assert (result == 1);



	uint8_t *found = bluetooth_cmd_search_devices();

	assert (found != NULL);

	int count = found[0];

	for (int i = 0; i < count; i++) {
		// Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
		squirrel_create_device_info_entry (found[1 + i * (16 + 6) + 16]);
	}

}

int master_main(void) {

}

int init(void) {
	int result = bluetooth_test_connection(4);
	assert (result == 1);
	master_discover();
}

/**
 * Command: ATR
 * Set mode of the device: master or slave.
 * The device will warm-start and clear all paired addresses.
 * @param mode Master = 0, Slave = 1
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_mode(uint8_t mode);

/**
 * Command: ATD
 * Set remote address to connecto to.
 * For security purpose, We can specifies the unique remote device can be connected. In
 * master role, it automatically inquire and search the slave even the slave is undiscoverable.
 * In slave role, the command should be as a filter condition to accept the master's inquiry.
 * If address is NULL, the remote address will be cleared.
 * @param address The remote address in the format 'xxxxxxxxxxxx' (12 Characters) or NULL to clear.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_remote_address(const uint8_t* address);

/**
 * Command: ATF?
 * Search bluetooth devices.
 * The data of found devices will be stored in bluetooth_found_devices.
 * bluetooth_found_devices_count will also be set.
 * @return Returns NULL on failure otherwise the array with found devices.
 *
 * @see bluetooth_data_package
 */
extern uint8_t* bluetooth_cmd_search_devices(void);

/**
 * Checks if it is already slave (1).
 * If not it switches to slave mode and disables autoconnect.
 * @return 1 on success, 0 on failure
 */
uint8_t setAsSlave()

extern uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length);

int main(void) {
	init();

	while (true) {
		if (mode == MASTER) {
			master_main();
		}

		if (package != NULL) {
			downlink_handle_package(package);
		}
	}
}

