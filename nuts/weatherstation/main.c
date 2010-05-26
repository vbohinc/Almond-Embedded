#include <bmp085.h>
#include <bluetooth/bluetooth.h>
#include <downlink/downlink.h>
#include <classes.h>

const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE};
const uint8_t class_id_extensions_length = 2;

void bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length);

int main (void)
{
  //TODO start bluetooth
  bluetooth_init(bluetooth_callback_handler);
  //TODO initialize sensors
  //TODO implement mainloop
  while(1)
  {
    //TODO use deep sleep
    //activate bluetooth 
    //TODO check variable for bluetooth 
    //process data 
  }
}

//functions for downlink protocol
//TODO implement this functon, make an container to save the data, and send the newest data.
uint16_t get_value(uint8_t id)
{
  switch(id) {
    case TEMPERATURE:
    break;
    case PRESSURE:
    break;
    default:
    break;
  }
  return 0;
}

void set_value(uint8_t id, uint16_t value)
{
  return; //no actor on this nut
}

void bluetooth_callback_handler (uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
  if (callback_type == 0) {
    downlink_handle_package(data_package);
  } else if (callback_type == 1) {
    // Connect
  } else if (callback_type == 2) {
    // Disconnect
  }
}


