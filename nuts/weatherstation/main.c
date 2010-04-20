#include <bmp085.h>
#include <bluetooth/bluetooth.h>
#include <downlink/downlink.h>
#include <classes.h>

const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE};
const uint8_t class_id_extensions_length = 2;


int main (void)
{
  //TODO start bluetooth
  //TODO initialize sensors
  //TODO implement mainloop
  while(1)
  {
    //TODO use deep sleep
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

