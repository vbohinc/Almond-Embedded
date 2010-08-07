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

uint16_t read_adc(uint8_t pin)
{
	// wandlung starten
	ADCSRA |= (1<<ADSC);
	// warten bis ergebniss
	while(ADCSRA & (1<<ADSC));
	
	return ADCW;
}

void init_adc(uint8_t pin, uint8_t factor)
{
	// Den ADC aktivieren und Teilungsfaktor auf 64 stellen
	ADCSRA = (1<<ADEN) | factor;

	// eingangspin waehlen
	ADMUX = pin;

	// referenzspannung waehlen
	ADMUX |= (1<<REFS1) | (1<<REFS0); // interne referenzspannung (also 2,56 V)
	//ADMUX |= (1<<REFS0); // avcc als referenzspannung
	//ADMUX |= 0;// aref als referenzspannung

	// erstes mal auslesen, dummyreadout
	read_adc(pin);
}

void close_adc()
{
	ADCSRA &= ~(1<<ADEN);
}


const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE, LED};
const uint8_t class_id_extensions_length = 3;

int main (void)
{
	//LED1_DDR |= 0xFF;

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
        //_delay_ms(1000);
	init_bmp085_sensor();


	//debug_pgm(PSTR("WTPG"));
	//mainloop
	while(1)
	{
//byte_to_hex(bmp085_get_data().temprature>>8);

/*
// bmp testauisgabe
		bmp_data_t d = bmp085_get_data();
	debug_pgm(PSTR("temp"));	
byte_to_hex(d.temprature>>8);
byte_to_hex(d.temprature);
	debug_pgm(PSTR("press"));
byte_to_hex(d.pressure>>8);
byte_to_hex(d.pressure);*/

debug_pgm(PSTR("lux"));
init_adc(0,(1<<ADPS1) | (1<<ADPS0));
uint16_t out = read_adc(0);
byte_to_hex(out>>8);
byte_to_hex((uint8_t)out);


debug_pgm(PSTR("hum"));
out = read_adc(1);
byte_to_hex(out>>8);
byte_to_hex((uint8_t)out);

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

