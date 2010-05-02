#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include <avr/io.h>
<<<<<<< HEAD:nuts/sensors/twi.h

void twi_start();

void twi_stop();
=======
#include <stdbool.h>


void twi_init(void);

//void twi_set_mode(); //only master read/write

enum twi_access_mode { READ=1, WRITE=0};

void twi_start(void);

void twi_connect(enum twi_access_mode, uint8_t);

uint8_t twi_status(void);

void twi_write(uint8_t);

uint8_t twi_read(void);

void twi_stop(void);
>>>>>>> origin/nuts:nuts/sensors/twi.h

#endif // TWI_H_INCLUDED

