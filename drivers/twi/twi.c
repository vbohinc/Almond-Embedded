/*
  TWI for Atmega 8535 an XMEGA
*/

#include <twi/twi.h>
#include <common.h>
#include <util/twi.h>
#include <error.h>

#define TWI TWIC

#if __AVR_ARCH__ <= 6
void twi_wait(void)
{
   while (!(TWCR & (1<<TWINT)));
}
#endif

/*void twi_setbitrate(uint16_t bitratekHz)
{
	uint8_t bitrate_div;
	// set i2c bitrate
	// Set Prescaler to one --> TWPS = 1
	TWSR =((0<<TWPS0)|(0<<TWPS1));

	//calculate bitrate division	
	bitrate_div = (((float)F_CPU/1000.0)/(float)bitratekHz);
	if(bitrate_div >= 16){
		bitrate_div = (float)(bitrate_div-16)/8;
	}
	TWBR=(uint8_t)bitrate_div;
	//TWBR=0; -> max speed
}*/

void twi_init(void) {
#if __AVR_ARCH__ <= 6
  //twi_setbitrate(1000);
  TWSR = 0;
  TWBR = 9;
  //this will give us 1000000/16 Hz clock for the twi interface
  TWCR = (1<<TWEN);
#elif __AVR_ARCH__ >= 100
  TWI.CTRL = 0;
  TWI.MASTER.BAUD = 0;           // maximum performance
  TWI.MASTER.CTRLA = 0 | (1<<3); // enable master, but disable interrupts
  TWI.MASTER.CTRLB = 0 | (1<<0); // enable smart mode
#endif
}

uint8_t twi_restart(void) {
#if __AVR_ARCH__ <= 6
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
  twi_wait();
  uint8_t status = twi_status();
  if(status == 0x08 || status == 0x10)
    return 0;
  else {
//debug_pgm(PSTR("error start twi"));
    return 1;
  }
#elif __AVR_ARCH__ >= 100
  //on xmega start is automatically send on connect
  TWI.MASTER.CTRLC = TWI.MASTER.CTRLC | (1<<0);
  return 0;
#endif
}

void twi_stop(void) {
#if __AVR_ARCH__ <= 6
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
#elif __AVR_ARCH__ >= 100
  TWI.MASTER.CTRLC = TWI.MASTER.CTRLC | (1<<0) | (1<<1);
#endif
}

uint8_t twi_connect(enum twi_access_mode mode, uint8_t addr)
{
#if __AVR_ARCH__ <= 6
  twi_restart();
  //check for valid status
  uint8_t status;// = twi_status();
  /*if(status == 0x08 || status == 0x10)
    return 0;
  else {
debug_pgm(PSTR("error connect1 twi"));
    return 1;
}*/
  TWDR = (addr<<1)|mode;
//byte_to_hex((addr<<1)|mode);
  TWCR = (1<<TWINT)|(1<<TWEN);

  twi_wait();
  status = twi_status();
  if(status == 0x18 || status == 0x20 || status == 0x40 || status == 0x48 || status == 0xF8)
    return 0;
  else {
//debug_pgm(PSTR("err twi_con"));
    return 1;
  }
#elif __AVR_ARCH__ >= 100
  TWI.MASTER.ADDR = (addr<<1)|mode;
  while((TWI.MASTER.STATUS & (1<<6)) == 0); //wait for WIF
  return 0;
#endif
}

uint8_t twi_write(uint8_t data)
{
#if __AVR_ARCH__ <= 6
  TWDR = data;
  TWCR = (1<<TWINT)|(1<<TWEN); 
  twi_wait();

uint8_t status = twi_status();


  if(status == 0x28 || status == 0x30)
    return 0;
  else {
//debug_pgm(PSTR("err twi_wr"));
    return 1;
  }
#elif __AVR_ARCH__ >= 100
  TWI.MASTER.DATA = data;
  while((TWI.MASTER.STATUS & (1<<6)) == 0); //wait for WIF
  return 0;
#endif
}

uint8_t twi_read(uint8_t* data, enum twi_send_ack ack)
{
#if __AVR_ARCH__ <= 6
  *data = TWDR;
  TWCR = (1<<TWINT)|(1<<TWEN)|(ack<<TWEA);
  twi_wait();
  uint8_t status = twi_status();
  if(status == 0x50 || status == 0x58)
    return 0;
  else {
//debug_pgm(PSTR("err twi_re"));
    return 1;
  }
#elif __AVR_ARCH__ >= 100
  TWI.MASTER.CTRLC = TWI.MASTER.CTRLC | ((ack==1)<<2);
  *data = TWI.MASTER.DATA;
  return 0;
#endif
}

// only needed on xmega, where you have to either stop restart or accept after a read(to commit the acknoledge)
void twi_accept()
{
#if __AVR_ARCH__ >= 100
  TWI.MASTER.CTRLC = TWI.MASTER.CTRLC | (1<<1);
#endif
}


//currently only working for atmega
uint8_t twi_status(void)
{
#if __AVR_ARCH__ <= 6
  //byte_to_hex(TWSR & 0xF8);
//uint8_t b = TWSR & 0xF8;
  return (TWSR & 0xF8); //mask of prescaler bits
#elif __AVR_ARCH__ >= 100
  return 0;
#endif
}




