/* Common */

#include <avr/interrupt.h>
#include <util/delay.h>
#include <common/timer.h>

/* Sensor */

#include <bmp085.h>

/* Downlink */

#include <bluetooth/bluetooth.h>
#include <downlink/downlink.h>
#include <classes.h>

#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED1_PIN 7

/* ADC */

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

int main (void)
{
	//init sleeping
	//init_timer();
	
	// FTDI - DEBUG!
  error_init();

  /* Start Bluetooth */
  bluetooth_init(downlink_bluetooth_callback_handler);
  sei();
  bluetooth_test_connection(4); //random number
  bluetooth_set_as_slave();
  
	/* Initialize Sensors */
  init_bmp085_sensor();


  debug_pgm(PSTR("LOOP"));
  while(1) {
    bluetooth_process_data();
    
		//activate bluetooth
		//TODO make meassurements
		//TODO check variable for bluetooth
		//TODO sleep again if no connection is recieved
		//process data
		// start_sleep(4); //TODO use a variable, instead of random number
		//_delay_ms(1000);
	}
}

/* Downlink */

const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE, LIGHT, HUMIDITY, LED};
const uint8_t class_id_extensions_length = 5;

uint16_t get_value(uint8_t id)
{
  bmp_data_t d;
  
  switch(id) {
    case 0: // TEMPERATURE
      d = bmp085_get_data();
      return (uint16_t) d.temprature;

    case 1: // PRESSURE
      d = bmp085_get_data();
      return (uint16_t) d.pressure;

    case 2: // LIGHT
      init_adc (0,(1<<ADPS1) | (1<<ADPS0));
      return read_adc(0);

    case 3: // HUMIDITY
      init_adc (1,(1<<ADPS1) | (1<<ADPS0));
      return read_adc (1);

    default:
      debug_pgm(PSTR("depp!"));
      return 0;
  }
}

void set_value(uint8_t id, uint16_t value)
{
  switch(id) {
	  case 4: // LED
		  if (value == 0)
				LED1_PORT &= ~(1<<LED1_PIN);
		  else
			  LED1_PORT |= (1<<LED1_PIN);
		  break;

	  default:
	    debug_pgm(PSTR("depp!"));
		  break;
  }
}

