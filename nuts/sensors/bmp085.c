/**
 * @brief sensor driver for the bmp085 pressure and temprature sensor
 * 
 */

#include <twi/twi.h>
#include <avr/eeprom.h>
#include <stdint.h>

struct temprature_conversion_data {
  uint16_t AC5;
  uint16_t AC6;
  int16_t MC;
  int16_t MD;
}

struct pressure_conversion_data {
  int16_t AC1;
  int16_t AC2;
  int16_t AC3;
  uint16_t AC4;
  int16_t B1;
  int16_t B2;
  //int16_t MB; //not needed in any calculation so won't use it
}



//the eeprom saved conversion data
struct temprature_conversion_data EEMEM tempconv;
struct press_conversion_data EEMEM pressconv;

uint16_t get_word(const uint8_t word)
{
  twi_start();
  twi_connect(WRITE,0x77);
  twi_write(word);
  twi_start();
  uint8_t temp;
  uint16_t result;
  twi_read(&temp,ACK);
  result = (temp<<8);
  twi_read(&temp,NACK);
  result += temp;
  twi_stop();
  return result;
}

uint8_t bmp085_get_data(uint16_t * temprature, uint16_t * pressure)
{
}

uint8_t get_conversion_data();
{
	
}
