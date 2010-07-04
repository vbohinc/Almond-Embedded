/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
      ___           ___           ___           ___           ___           ___     
     /  /\         /  /\         /__/\         /__/\         /  /\         /__/\    
    /  /:/        /  /::\       |  |::\       |  |::\       /  /::\        \  \:\   
   /  /:/        /  /:/\:\      |  |:|:\      |  |:|:\     /  /:/\:\        \  \:\  
  /  /:/  ___   /  /:/  \:\   __|__|:|\:\   __|__|:|\:\   /  /:/  \:\   _____\__\:\ 
 /__/:/  /  /\ /__/:/ \__\:\ /__/::::| \:\ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\
 \  \:\ /  /:/ \  \:\ /  /:/ \  \:\~~\__\/ \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/
  \  \:\  /:/   \  \:\  /:/   \  \:\        \  \:\        \  \:\  /:/   \  \:\  ~~~ 
   \  \:\/:/     \  \:\/:/     \  \:\        \  \:\        \  \:\/:/     \  \:\     
    \  \::/       \  \::/       \  \:\        \  \:\        \  \::/       \  \:\    
     \__\/         \__\/         \__\/         \__\/         \__\/         \__\/    


*
* Error handling functions.
*/

#ifndef __ERROR__
#define __ERROR__

#include "common.h"
#include <avr/pgmspace.h>



#ifdef DEBUG

void error_init(void);

void error_putc(const char c);

void assert (bool condition, const char *msg);
void info (const char *msg);
void warn(const char *msg);
void debug(const char *msg);
void error(const char *msg);

void assert_pgm(bool condition, const prog_char *msg);
void info_pgm (const prog_char *msg);
void warn_pgm(const prog_char *msg);
void debug_pgm(const prog_char *msg);
void error_pgm(const prog_char *msg);

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

#endif

#endif
