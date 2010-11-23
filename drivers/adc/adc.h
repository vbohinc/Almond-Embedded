/**
 * Driver for the ATMega ADC
 * @file adc.h
 * @author Maximilian Karl
 */

#ifndef _NUTS_ADC_H
#define _NUTS_ADC_H

#include <avr/io.h>

// prescaler divison factor
#define ADC_DF2   (1<<ADPS0)
#define ADC_DF4   (1<<ADPS1)
#define ADC_DF8   (1<<ADPS1) | (1<<ADPS0)
#define ADC_DF16  (1<<ADPS2)
#define ADC_DF32  (1<<ADPS2) | (1<<ADPS0)
#define ADC_DF64  (1<<ADPS2) | (1<<ADPS1
#define ADC_DF128 (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)

/**
* \brief Sets ADC for pint with factor
* Set Analog Digital Converter with factor for a pin 
* @param pin to activate adc on
* @param factor
*/
void init_adc(uint8_t pin, uint8_t factor);

/**
* \brief read the adc of a pin
* reads the adc vlaue of a pin
* @param pin the pin to read from
* @return the adc converted value of the pin 
*/
uint16_t read_adc(uint8_t pin);

/** \brief close adc
* Closes the Analog Digital Converter
*/
void close_adc();

#endif // _NUTS_ADC_H
