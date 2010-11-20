/**
 * error_driver.h - Error output Driver
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#ifndef __ERROR_DRIVER__
#define __ERROR_DRIVER__

#include <avr/io.h>

#ifndef X86
#ifdef DEBUG
extern void error_driver_write_c (uint8_t c);
extern void error_driver_init (void);
#else
#define error_driver_write_c(c) {}
#define error_driver_init() {}
#endif
#endif

#endif //__ERROR_DRIVER__
