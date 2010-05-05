#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include <avr/io.h>
#include <stdbool.h>

void twi_init(void);

enum twi_access_mode { READ=1, WRITE=0};

uint8_t twi_start(void);

uint8_t twi_stop(void);

uint8_t twi_connect(enum twi_access_mode, uint8_t);

uint8_t twi_write(uint8_t);

uint8_t twi_read(uint8_t *);

uint8_t twi_status(void);

#endif // TWI_H_INCLUDED

