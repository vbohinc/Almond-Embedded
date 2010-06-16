#include "../protocols/downlink/downlink.h"
#include "../protocols/uplink/uplink.h"
#include "../drivers/bluetooth/bluetooth.h"
static void bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {
		// Data package
		if (data_length == DOWNLINK_PACKAGE_LENGTH) {
			//downlink_handle_package(data_package);
		} else if (data_length == UPLINK_PACKAGE_LENGTH) {
			uplink_handle_package(data_package);
		}
	} else if (callback_type == 1) {
		// Connect
	} else if (callback_type == 2) {
		// Disconnect
	}
}

static void init_bluetooth (void) {
	bluetooth_init(bluetooth_callback_handler);
	int result = bluetooth_test_connection(4);
	assert (result == 1, "Could not test the connection");
	result = bluetooth_set_as_master();
	assert (result == 1, "Could not set master mode");
}

static void downlink_discover(void) {
	uint8_t *found = bluetooth_cmd_search_devices();

	assert (found != NULL, "Malformed search result");

	int count = found[0];

	for (int i = 0; i < count; i++) {
		// Data Structure COUNT (1) | NAME (16) | MAC (6) | ...
		squirrel_create_device_info_entry (found[1 + i * (16 + 6) + 16]);
	}

}


static void init_downlink (void) {
	//downlink_discover();
	uint8_t *address = malloc(12*sizeof(uint8_t));
	uint8_t address_bytes[6] = {0x00, 0x12, 0x6f, 0x03, 0x70, 0x95};
	bluetooth_address_to_array(address, address_bytes, 0, 0, 0);
	bluetooth_cmd_set_remote_address(address);
	bluetooth_cmd_connect(0);
	uint8_t class = downlink_get_nut_class(NULL);
}


static int main (void) {
	init_bluetooth ();
	//init_display ();
	//init_storage ();
	init_downlink ();
}
