/*
* Functions to write error message to FTDI or USART
*/

#ifndef __ERROR_DRIVER__
#define __ERROR_DRIVER__

#include <avr/io.h>

#ifndef X86
#ifdef DEBUG
extern void error_driver_write_c(uint8_t c);
extern void error_driver_init(void);
#else
#define error_driver_write_c(c) {}
#define error_driver_init() {}
#endif
#endif

#endif //__ERROR_DRIVER__
