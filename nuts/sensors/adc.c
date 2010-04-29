#include "adc.h"

void init_adc(uchar8_t pin, uchar8_t factor)
{
	// Den ADC aktivieren und Teilungsfaktor auf 64 stellen
	ADCSRA = (1<<ADEN) | divisionalfactor;

	// eingangspin waehlen
	ADMUX = pin;

	// referenzspannung waehlen
	ADMUX |= (1<<REFS1) | (1<<REFS0); // interne referenzspannung (also 2,56 V)
	//ADMUX |= (1<<REFS0); // avcc als referenzspannung
	//ADMUX |= 0;// aref als referenzspannung

	// erstes mal auslesen, dummyreadout
	read_adc(pin);
}

uchar16_t read_adc(uchar8_t pin)
{
	// wandlung starten
	ADCSRA |= (1<<ADSC);
	// warten bis ergebniss
	while(ADCSRA & (1<<ADSC));
	
	return ADCW;
}

void close_adc()
{
	ADCSRA &= ~(1<<ADEN);
}