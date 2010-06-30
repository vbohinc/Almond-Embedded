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
      ___           ___           ___                       ___           ___           ___           ___                 
     /__/\         /  /\         /  /\          ___        /__/\         /  /\         /  /\         /  /\          ___   
    _\_ \:\       /  /:/_       /  /::\        /  /\       \  \:\       /  /:/_       /  /::\       /  /:/_        /  /\  
   /__/\ \:\     /  /:/ /\     /  /:/\:\      /  /:/        \__\:\     /  /:/ /\     /  /:/\:\     /  /:/ /\      /  /:/  
  _\_ \:\ \:\   /  /:/ /:/_   /  /:/~/::\    /  /:/     ___ /  /::\   /  /:/ /:/_   /  /:/~/:/    /  /:/ /::\    /  /:/   
 /__/\ \:\ \:\ /__/:/ /:/ /\ /__/:/ /:/\:\  /  /::\    /__/\  /:/\:\ /__/:/ /:/ /\ /__/:/ /:/___ /__/:/ /:/\:\  /  /::\   
 \  \:\ \:\/:/ \  \:\/:/ /:/ \  \:\/:/__\/ /__/:/\:\   \  \:\/:/__\/ \  \:\/:/ /:/ \  \:\/:::::/ \  \:\/:/~/:/ /__/:/\:\  
  \  \:\ \::/   \  \::/ /:/   \  \::/      \__\/  \:\   \  \::/       \  \::/ /:/   \  \::/~~~~   \  \::/ /:/  \__\/  \:\ 
   \  \:\/:/     \  \:\/:/     \  \:\           \  \:\   \  \:\        \  \:\/:/     \  \:\        \__\/ /:/        \  \:\
    \  \::/       \  \::/       \  \:\           \__\/    \  \:\        \  \::/       \  \:\         /__/:/          \__\/
     \__\/         \__\/         \__\/                     \__\/         \__\/         \__\/         \__\/                
      ___                                   ___           ___     
     /  /\          ___       ___          /  /\         /__/\    
    /  /::\        /  /\     /  /\        /  /::\        \  \:\   
   /  /:/\:\      /  /:/    /  /:/       /  /:/\:\        \  \:\  
  /  /:/~/::\    /  /:/    /__/::\      /  /:/  \:\   _____\__\:\ 
 /__/:/ /:/\:\  /  /::\    \__\/\:\__  /__/:/ \__\:\ /__/::::::::\
 \  \:\/:/__\/ /__/:/\:\      \  \:\/\ \  \:\ /  /:/ \  \:\~~\~~\/
  \  \::/      \__\/  \:\      \__\::/  \  \:\  /:/   \  \:\  ~~~ 
   \  \:\           \  \:\     /__/:/    \  \:\/:/     \  \:\     
    \  \:\           \__\/     \__\/      \  \::/       \  \:\    
     \__\/                                 \__\/         \__\/    
*/

#include <bmp085.h>
#include <bluetooth/bluetooth.h>
#include <downlink/downlink.h>
#include <classes.h>
#include <common/timer.h>
#include <avr/interrupt.h>

const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE};
const uint8_t class_id_extensions_length = 2;

int main (void)
{
  //init sleeping
  init_timer();
  //start bluetooth
  bluetooth_init(downlink_bluetooth_callback_handler);
  bluetooth_test_connection(4); //random number
  bluetooth_set_as_slave();
  //initialize sensors
  init_bmp085_sensor();
  //mainloop
  while(1)
  {
    //activate bluetooth 
    //TODO make meassurements
    //TODO check variable for bluetooth 
    //TODO sleep again if no connection is recieved
    //process data

    start_sleep(4); //TODO use a variable, instead of random number 
  }
}

//functions for downlink protocol
//TODO implement this functon, make an container and send the newest data.
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

