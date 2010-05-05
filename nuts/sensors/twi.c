#include <../sensors/twi.h>
#include <util/twi.h>

void twi_init(void) {
  TWSR = 0;
  TWBR = 0;
  //this will give us 1000000/16 Hz clock for the twi interface
}

void twi_start(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
  twi_wait();
}

void twi_stop(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}

void twi_connect(enum twi_access_mode mode, uint8_t addr)
{
  TWDR = (addr<<1)|mode;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
}

void twi_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
}

uint8_t twi_read(void)
{
  uint8_t result = TWDR;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  return result;
}

uint8_t twi_status(void)
{
  return (TWSR & 0xF8); //mask of prescaler bits
}

void twi_wait(void)
{
   while (!(TWCR & (1<<TWINT)));
}

