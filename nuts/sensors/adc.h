
#ifndef _NUTS_ADC_H
#define _NUTS_ADC_H

#include <avr/io.h>

// prescaler divison faktor
#define ADC_DF2   (1<<ADPS0)
#define ADC_DF4   (1<<ADPS1)
#define ADC_DF8   (1<<ADPS1) | (1<<ADPS0)
#define ADC_DF16  (1<<ADPS2)
#define ADC_DF32  (1<<ADPS2) | (1<<ADPS0)
#define ADC_DF64  (1<<ADPS2) | (1<<ADPS1
#define ADC_DF128 (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)


void init_adc(uchar8_t pin, uchar8_t factor);
uchar16_t read_adc(uchar8_t pin);
void close_adc();

#endif // _NUTS_ADC_H