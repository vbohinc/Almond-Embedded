/**
 * @brief sensor driver for the bmp085 pressure and temprature sensor
 * 
 */

#include <twi.h>
#include <avr/eeprom.h>
#include <stdint.h>

#include "bmp085.h"

struct temprature_conversion_data {
  uint16_t AC5;
  uint16_t AC6;
  int16_t MC;
  int16_t MD;
};

struct pressure_conversion_data {
  int16_t AC1;
  int16_t AC2;
  int16_t AC3;
  uint16_t AC4;
  int16_t B1;
  int16_t B2;
  //int16_t MB; //not needed in any calculation so won't use it
};



//the eeprom saved conversion data
const struct temprature_conversion_data tempconv EEMEM;
const struct pressure_conversion_data pressconv EEMEM;

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

//uint8_t bmp085_get_data(uint16_t * temprature, uint16_t * pressure)
//{
//}

void get_conversion_data(void)
{
  struct temprature_conversion_data temptable;
  struct pressure_conversion_data presstable;
  //temprature
  temptable.AC5 = get_word(0xB2);
  temptable.AC6 = get_word(0xB4);
  temptable.MC  = get_word(0xBC);
  temptable.MD  = get_word(0xBE);
  eeprom_write_block(&temptable,&tempconv,sizeof(struct temprature_conversion_data));
  //pressure
  presstable.AC1 = get_word(0xAA);
  presstable.AC2 = get_word(0xAC);
  presstable.AC3 = get_word(0xAE);
  presstable.AC4 = get_word(0xB0);
  presstable.B1  = get_word(0xB6);
  presstable.B2  = get_word(0xB8);
  eeprom_write_block(&presstable,&pressconv,sizeof(struct pressure_conversion_data));
}

