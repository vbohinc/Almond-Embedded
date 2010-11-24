/**
 * header for the error output driver
 * @file error.h
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
#ifndef X86
#include <avr/pgmspace.h>
#endif


#ifdef DEBUG

/// Initializes error management
void error_init (void);

/** Prints a char as error message
* @param c char to be printed
*/
void error_putc (const char c);

/** Asserts a certain condition
* @param condition condition to be asserted
* @param msg message to be send, when condition fails
*/
void assert (bool condition, const char *msg);

/** Sends an information message
* @param msg message to be send
*/
void info (const char *msg);

/** Sends an warning message
* @param msg message to be send
*/
void warn (const char *msg);

/** Sends an debug message
* @param msg message to be send
*/
void debug (const char *msg);

/** Sends an error message
* @param msg message to be send
*/
void error (const char *msg);

/** Sends an error message
* @param msg message to be send
*/
void assert_pgm (bool condition, const prog_char *msg);

/** Writes an info message to program memory
* @param msg message to be written
*/
void info_pgm (const prog_char *msg);

/** Writes an warning message to program memory
* @param msg message to be written
*/
void warn_pgm (const prog_char *msg);

/** Writes an debug message to program memory
* @param msg message to be written
*/
void debug_pgm (const prog_char *msg);

/** Writes an error message to program memory
* @param msg message to be written
*/
void error_pgm (const prog_char *msg);

/** Prints a byte value as hexadecimal value
* @param byte byte value to be printed
*/
void byte_to_hex (uint8_t byte);

#else

#define error_init() {}


#define error_putc(c) {}

#define assert(cond, msg) {}
#define info(msg) {}
#define warn(msg) {}
#define debug(msg) {}
#define error(msg) {}

#define assert_pgm(cond, msg) {}
#define info_pgm(msg) {}
#define warn_pgm(msg) {}
#define debug_pgm(msg) {}
#define error_pgm(msg) {}
#define byte_to_hex(byte) {}
#endif
#endif

/**
 * Converts a char value 0-9,A-F,a-f to its corresponding int value
 * @param chr the char value
 * @return the int value
 */
extern uint8_t char_to_hex (uint8_t chr);

/**
 * Converts a hex value to its corresponding char value 0-9,A-F
 * @param hex the hex value
 * @return the char value
 */
extern uint8_t hex_to_char (uint8_t hex);
