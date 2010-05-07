#include <twi.h>
#include <util/twi.h>

void twi_wait(void);

void twi_init(void) {
  TWSR = 0;
  TWBR = 0;
  //this will give us 1000000/16 Hz clock for the twi interface
}

uint8_t twi_start(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
  twi_wait();
  uint8_t status = twi_status();
  if(status == 0x08 || status == 0x10)
    return 0;
  else
    return -1;
}

void twi_stop(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
  twi_wait();
}

uint8_t twi_connect(enum twi_access_mode mode, uint8_t addr)
{
  //check for valid status
  uint8_t status = twi_status();
  if(status == 0x08 || status == 0x10)
    return 0;
  else
    return -1;
  TWDR = (addr<<1)|mode;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  status = twi_status();
  if(status == 0x18 || status == 0x20 || status == 0x40 || status == 0x48)
    return 0;
  else
    return -1;
}

uint8_t twi_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  uint8_t status = twi_status();
  if(status == 0x28 || status == 0x30)
    return 0;
  else
    return -1;

}

uint8_t twi_read(uint8_t* data)
{
  //TODO check for a correct result
  *data = TWDR;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  uint8_t status = twi_status();
  if(status == 0x50 || status == 0x58)
    return 0;
  else
    return -1;
}

uint8_t twi_status(void)
{
  return (TWSR & 0xF8); //mask of prescaler bits
}

void twi_wait(void)
{
   while (!(TWCR & (1<<TWINT)));
}

