#include <twi.h>
#include <util/twi.h>

//TODO make all functions return 0 on success and -1 on failure, use twi_status to get error

/**
 * see below, waits for operation to finish
 */
void twi_wait(void);

/**
 * set Frequency
 */
void twi_init(void) {
  TWSR = 0;
  TWBR = 0;
  //this will give us 1000000/16 Hz clock for the twi interface
}

/**
 * Send TWI Start signal
 */
uint8_t twi_start(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
  twi_wait();
  return twi_status();
}

/**
 * Send TWI Stop signal
 */
uint8_t twi_stop(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
  twi_wait();
  return twi_status();
}

/**
 * Generate a SLA+R / SLA+W and send it
 */
uint8_t twi_connect(enum twi_access_mode mode, uint8_t addr)
{
  TWDR = (addr<<1)|mode;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  return twi_status();
}

/**
 * Send data to Slave (Only if SLA+W was sent, ILLEGAL otherwise!)
 */
uint8_t twi_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  return twi_status();
}

/**
 * Get recieved Data
 */
uint8_t twi_read(uint8_t* data)
{
  //TODO check for a correct result
  *data = TWDR;
  TWCR = (1<<TWINT)|(1<<TWEN);
  twi_wait();
  return twi_status();
}

/**
 * get status
 */
uint8_t twi_status(void)
{
  return (TWSR & 0xF8); //mask of prescaler bits
}

/**
 * wait for command to finish
 */
void twi_wait(void)
{
   while (!(TWCR & (1<<TWINT)));
}

