/**
 * adc.c - Driver for the ATMega ADC
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Maximilian Karl
 */

#include "adc.h"

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

uint16_t read_adc(uint8_t pin)
{
    // wandlung starten
    ADCSRA |= (1<<ADSC);
    // warten bis ergebnis
    while (ADCSRA & (1<<ADSC));

    return ADCW;
}

void close_adc()
{
    ADCSRA &= ~(1<<ADEN);
}
