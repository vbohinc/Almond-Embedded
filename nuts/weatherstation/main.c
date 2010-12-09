/**
 * main.c - weatherstation NUT
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

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
#define BLUETOOTH_START_TIME 5

/* ADC */

uint16_t read_adc (uint8_t pin)
{
    // wandlung starten
    ADCSRA |= (1 << ADSC);
    // warten bis ergebniss
    while (ADCSRA & (1 << ADSC));

    return ADCW;
}

void init_adc (uint8_t pin, uint8_t factor)
{
    // Den ADC aktivieren und Teilungsfaktor auf 64 stellen
    ADCSRA = (1 << ADEN) | factor;

    // eingangspin waehlen
    ADMUX = pin;

    // referenzspannung waehlen
    ADMUX |= (1 << REFS1) | (1 << REFS0); // interne referenzspannung (also 2,56 V)
    //ADMUX |= (1<<REFS0); // avcc als referenzspannung
    //ADMUX |= 0;// aref als referenzspannung

    // erstes mal auslesen, dummyreadout
    read_adc (pin);
}

void close_adc (void)
{
    ADCSRA &= ~ (1 << ADEN);
}

/* Downlink */

uint16_t sleep = 0;
const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE, LIGHT, HUMIDITY, LED};
const uint8_t class_id_extensions_length = 5;

uint16_t get_value (uint8_t id)
{
    bmp_data_t d;

    switch (id)
    {

        case 0: // TEMPERATURE
            d = bmp085_get_data();
            return 0;//(uint16_t) d.temprature;

        case 1: // PRESSURE
            d = bmp085_get_data();
            return 0;//(uint16_t) d.pressure;

        case 2: // LIGHT
            {
                init_adc (0, (1 << ADPS1) | (1 << ADPS0));
                double a = (double) read_adc (0);
                a = (226.383 * (a - 468.995) * (a - 88.086) * (a - 9.53595)) / (a * a * a);
                return (uint16_t) a; // adc value to lux (from 0.1 to 1000)
            }

        case 3: // HUMIDITY
            {
                init_adc (1, (1 << ADPS1) | (1 << ADPS0));
                double a = (double) read_adc (1);
                a = (125 * a) / 768 - 26.6667;
                return (uint16_t) a; // adc value to percent (from 0 to 100)
            }

        default:
            debug_pgm (PSTR ("UNK:SEN"));
            return 0;
    }
}

void set_value (uint8_t id, uint16_t value)
{
    switch (id)
    {
        case 4: // LED
            if (value == 0)
                LED1_PORT &= ~ (1 << LED1_PIN);
            else
                LED1_PORT |= (1 << LED1_PIN);

            break;

        default:
            debug_pgm (PSTR ("UNK:AKT"));

            break;
    }
}

void disable_bt (void)
{
    cli ();
#ifndef DEBUG
    set_bit (PORTB, 0);
#endif
}

bool enable_bt (void)
{
    sei ();
#ifndef DEBUG
    set_bit (DDRB, 0);
    set_bit (PORTB, 0);
    _delay_ms (50);
    clear_bit (PORTB, 0);
#endif
    return bt_init (NULL);
}

int main (void)
{
#ifdef DEBUG
    /* FTDI */
    error_init ();
#endif

    /* Initialize Actors */
    LED1_DDR |= (1 << LED1_PIN);

    /* Switch on actor Led */

    /* Initialize Bluetooth */

    if (!enable_bt ())
    {
        // switch led on on error
        set_value (4, 1);
    }

    /* Initialize Sensors */
    init_bmp085_sensor ();

    while (true)
    {
        uint8_t data[DOWNLINK_PACKAGE_LENGTH];

        if (bt_receive (data, DOWNLINK_PACKAGE_LENGTH, 0))
            downlink_process_pkg (data);

        if (sleep > BLUETOOTH_START_TIME)
        {
            disable_bt ();

            for (; sleep > BLUETOOTH_START_TIME; sleep--)
                _delay_ms (1000);

            enable_bt ();
        }
    }
}
