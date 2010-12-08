
#include <twi/twi.h>
#include <util/twi.h>
#include <error.h>

void twi_wait(void);

void twi_setbitrate(uint16_t bitratekHz)
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
}

void twi_init(void) {
  //twi_setbitrate(1000);
  TWSR = 0;
  TWBR = 9;
  //this will give us 1000000/16 Hz clock for the twi interface
  TWCR = (1<<TWEN);

}

uint8_t twi_start(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);
  twi_wait();
  uint8_t status = twi_status();
  if(status == 0x08 || status == 0x10)
    return 0;
  else {
	PORTD |= (1<<7);
debug_pgm(PSTR("error start twi"));
    return 1;
}
}

void twi_stop(void) {
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
  //twi_wait();
}

uint8_t twi_connect(enum twi_access_mode mode, uint8_t addr)
{
  //check for valid status
  uint8_t status;// = twi_status();
  /*if(status == 0x08 || status == 0x10)
    return 0;
  else {
debug_pgm(PSTR("error connect1 twi"));
    return 1;
}*/
  TWDR = (addr<<1)|mode;
byte_to_hex((addr<<1)|mode);
  TWCR = (1<<TWINT)|(1<<TWEN);

  twi_wait();
  status = twi_status();
  if(status == 0x18 || status == 0x20 || status == 0x40 || status == 0x48 || status == 0xF8)
    return 0;
  else {
PORTD |= (1<<7);
debug_pgm(PSTR("err twi_con"));
    return 1;
  
}
}

uint8_t twi_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1<<TWINT)|(1<<TWEN); 
  twi_wait();

uint8_t status = twi_status();

  if(status == 0x28 || status == 0x30)
    return 0;
  else {
PORTD |= (1<<7);
debug_pgm(PSTR("err twi_wr"));
    return 1;
}

}

uint8_t twi_read(uint8_t* data, enum twi_send_ack ack)
{
  TWCR = (1<<TWINT)|(1<<TWEN)|(ack<<TWEA);
  twi_wait();
  *data = TWDR;
  uint8_t status = twi_status();
  if(status == 0x50 || status == 0x58)
    return 0;
  else {
PORTD |= (1<<7);
debug_pgm(PSTR("err twi_re"));
    return 1;
}
}

uint8_t twi_status(void)
{
  //byte_to_hex(TWSR & 0xF8);
//uint8_t b = TWSR & 0xF8;
  return (TWSR & 0xF8); //mask of prescaler bits
}

void twi_wait(void)
{
PORTD |= (1<<7);
//debug_pgm(PSTR("twi waiting"));
   while (!(TWCR & (1<<TWINT)));
PORTD &= ~(1<<7);
}

