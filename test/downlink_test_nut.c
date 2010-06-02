#include <stdio.h>
#include "../protocols/downlink/downlink.h"
#include "../shared/common.h"

#define NUT

const uint8_t class_id_nut = 	WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE, HUMIDITY};
const uint8_t class_id_extensions_length = 3;

uint16_t get_value (uint8_t id) {
  return 13;
}

void set_value (uint8_t id, uint16_t value) {

}

void downlink_bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length) {
	printf("Nut callback handler called!\n");
	printf("%d", data_length);
}
int main()
{
	bluetooth_init (downlink_bluetooth_callback_handler);
	int result = bluetooth_test_connection (4);
	//assert (result == 1, "Could not test the connection");
	result = bluetooth_set_as_slave ();
	//assert (result == 1, "Could not set slave mode");
	printf("Done init");
  while (1) {
	  //printf("Processing data!\n");
	  bluetooth_process_data();
  }
	bluetooth_close();
}

