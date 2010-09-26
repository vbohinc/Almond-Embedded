/* Common */

#include <avr/interrupt.h>
#include <util/delay.h>

/* Sensor */

#include <bmp085.h>

/* Downlink */

#include <bluetooth/bluetooth.h>
#include <downlink/downlink.h>
#include <classes.h>

#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED1_PIN 7
#define BLUETOOTH_START_TIME 2

/* ADC */

uint16_t read_adc(uint8_t pin)
{
  // wandlung starten
  ADCSRA |= (1<<ADSC);
  // warten bis ergebniss
  while (ADCSRA & (1<<ADSC));

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

void close_adc(void)
{
  ADCSRA &= ~(1<<ADEN);
}

/* Downlink */

uint16_t sleep = 0;
const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE, LIGHT, HUMIDITY, LED};
const uint8_t class_id_extensions_length = 5;

uint16_t get_value(uint8_t id)
{
  bmp_data_t d;

  switch (id)
    {
    case 0: // TEMPERATURE
      d = bmp085_get_data();
      return (uint16_t) d.temprature;

    case 1: // PRESSURE
      d = bmp085_get_data();
      return (uint16_t) d.pressure;

    case 2: // LIGHT
      init_adc (0,(1<<ADPS1) | (1<<ADPS0));
      return read_adc (0);

    case 3: // HUMIDITY
      init_adc (1,(1<<ADPS1) | (1<<ADPS0));
      return read_adc (1);

    default:
      debug_pgm(PSTR("UNK:SEN"));
      return 0;
    }
}

void set_value(uint8_t id, uint16_t value)
{
  switch (id)
    {
    case 4: // LED
      if (value == 0)
        LED1_PORT &= ~(1<<LED1_PIN);
      else
        LED1_PORT |= (1<<LED1_PIN);
      break;

    default:
      debug_pgm(PSTR("UNK:AKT"));
      break;
    }
}

void blue_sky (void)
{
  sei();
  bt_init();
  bt_set_mode(BLUETOOTH_SLAVE);
}

int main (void)
{
  /* FTDI */
  error_init ();

  /* Initialize Bluetooth */
  blue_sky ();

  /* Initialize Sensors */
  init_bmp085_sensor ();

  /* Initialize Actors */
  LED1_DDR |= (1<<LED1_PIN);
  LED1_PORT |= (1<<LED1_PIN);

  while (true)
    {
      //insert downlink function, that handles bluetooth

      if (sleep > BLUETOOTH_START_TIME)
        {
          LED1_PORT &= ~(1<<LED1_PIN);
          for (; sleep > BLUETOOTH_START_TIME; sleep--)
            _delay_ms(1000);

          blue_sky();
          LED1_PORT |= (1<<LED1_PIN);
        }
    }
}
