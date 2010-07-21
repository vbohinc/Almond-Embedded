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
#include <util/delay.h>

#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED2_DDR DDRC
#define LED2_PORT PORTC
#define LED1_PIN 7
#define LED2_PIN 0


const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE, LED};
const uint8_t class_id_extensions_length = 3;

int main (void)
{

//	LED1_DDR |= 0xFF;

	//LED2_DDR |= 0xFF;
	/*LED1_PORT |= (1<<LED1_PIN);
	LED2_PORT |= (1<<LED2_PIN);
	_delay_ms(500);
	LED1_PORT &= ~(1<<LED1_PIN);
	LED2_PORT &= ~(1<<LED2_PIN);
	_delay_ms(500);*/


  //init sleeping
  
  //init_timer();
  error_init();
	//debug_pgm(PSTR("#NUT#"));
  //start bluetooth
  //bluetooth_init(downlink_bluetooth_callback_handler);
  //sei();
  //bluetooth_test_connection(4); //random number
  //bluetooth_set_as_slave();
  //initialize sensors
  init_bmp085_sensor();


	//debug_pgm(PSTR("WTPG"));
  //mainloop
  while(1)
  {
    bmp085_get_data();
    //activate bluetooth 
    //TODO make meassurements
    //TODO check variable for bluetooth 
    //TODO sleep again if no connection is recieved
    //process data
	  //bluetooth_process_data();

   // start_sleep(4); //TODO use a variable, instead of random number
		//_delay_ms(1000);
  }
}

//functions for downlink protocol
//TODO implement this functon, make an container and send the newest data.
uint16_t get_value(uint8_t id)
{
  switch(id) {
    case 0: // TEMP
    	return (uint16_t) 366;
    break;
    case 1: // PRESS
    	return (uint16_t) 1000;
    break;
    default:
    	// DEBUG!!!
    break;
  }
  return 0;
}

void set_value(uint8_t id, uint16_t value)
{

  switch(id) {
	  case 2: // LED
		  if (value == 0)
				LED1_PORT &= ~(1<<LED1_PIN);
		  else
			  LED1_PORT |= (1<<LED1_PIN);
		  break;

	  default:
		  break;

  }
}

