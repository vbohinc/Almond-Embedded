/**
 * Error output Driver
 * @file error_driver.h
 */

#ifndef __ERROR_DRIVER__
#define __ERROR_DRIVER__

#include <avr/io.h>

#ifndef X86
#ifdef DEBUG

/**
* writes a char c on output device (on squirrel display)
* @param c char to write
*/
extern void error_driver_write_c (uint8_t c);

/**
* Initializes error output driver(on squirrel display)
*/
extern void error_driver_init (void);
#else
#define error_driver_write_c(c) {}
#define error_driver_init() {}
#endif
#endif

#endif //__ERROR_DRIVER__
