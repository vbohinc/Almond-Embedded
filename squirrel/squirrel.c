/*
 * squirrel.c
 *
 */
#include "squirrel.h"

enum state {
	SLAVE, MASTER
};

struct device_info {
		uint8_t mac[6];
		uint8_t class;
		uint8_t actuator_types[16];
		uint8_t sensor_types[16];
		uint8_t config_types[16];
};

struct device_info *device_list[16];


int master_discover (void) {
		uint8_t *found = bluetooth_cmd_search_devices();
		if (found == NULL)
		    printf("SEARCH ERROR!");
		else {
		    int count = found[0];
		    printf("\nCount=%d", count);
		    for (int i=0; i<count; i++)
		    {
		    		printf("\n%d: '", i+1);
		    		for (int j=0; j<16; j++)
				{
		  			printf("%c", found[1+i*(16+6)+j]);
				}
				printf("' ");
				for (int j=0; j<6; j++)
		  			printf("%c", found[1+i*(16+6)+16+j]);
				uint8_t found_mac[6] = found[1+i*(16+6)+16]; // Assuming the MAC address is stored as a uint8_t[6] in found_mac
				bool device_already_exists = false;
				int k;
				for (k=0; k<16; k++) {
					if (device_list[k] == NULL) { // We haven't found the MAC, time to create a new entry
						break;
					}
					if (found_mac == device_list[k]->mac) { // Already there, nothing to do
						device_already_exists = true;
						break;
					}
				}
				if (!device_already_exists) {
					device_list[k]->mac = found_mac;
					device_list[k]->class = downlink_get_class();
					for (int j=0; j < 16; j++) {
						device_list[k]->actuator_types[j] = downlink_get_actuator_class(j+0x80); // Actuators begin at ID 0x80
					}
					for (int j=0; j < 16; j++) {
						device_list[k]->sensor_types[j] = downlink_get_sensor_class(j);
					} // TODO config_types
				} // And we're done creating a new device entry
				
		  	}
		}
}

int master_main (void) {

}

int init(void) {
  master_discover ();
}

/**
 * Command: ATR
 * Set mode of the device: master or slave.
 * The device will warm-start and clear all paired addresses.
 * @param mode Master = 0, Slave = 1
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_mode (uint8_t mode);

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
extern uint8_t bluetooth_cmd_set_remote_address (const uint8_t* address);


/**
 * Command: ATF?
 * Search bluetooth devices.
 * The data of found devices will be stored in bluetooth_found_devices.
 * bluetooth_found_devices_count will also be set.
 * @return Returns NULL on failure otherwise the array with found devices.
 *
 * @see bluetooth_data_package
 */
extern uint8_t* bluetooth_cmd_search_devices (void);

/**
 * Checks if it is already slave (1).
 * If not it switches to slave mode and disables autoconnect.
 * @return 1 on success, 0 on failure
 */
uint8_t setAsSlave()


extern uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length);


int main(void) {
		init();

		while(true) {
			if (mode == MASTER) {
				master_main();
			}

			if (package != NULL) {
				downlink_handle_package(package);
			}
		}
}



