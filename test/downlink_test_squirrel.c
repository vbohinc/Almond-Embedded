#include "../protocols/downlink/downlink.h"
#include "../drivers/bluetooth/bluetooth.h"
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

void downlink_discover(void) {
	uint8_t *found = bluetooth_cmd_search_devices();

	assert (found != NULL, "Malformed search result");

	int count = found[0];

	for (int i = 0; i < count; i++) {
		// Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
		squirrel_create_device_info_entry (found[1 + i * (16 + 6) + 16]);
	}

}


void init_downlink (void) {
	downlink_discover();
}


int main (void) {
	init_bluetooth ();
	//init_display ();
	//init_storage ();
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
