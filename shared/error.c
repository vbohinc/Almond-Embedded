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

 
 **
 * Error handling functions
 */

#include <stdbool.h>
#include "ftdi.h"

#include <avr/pgmspace.h>
#include "error_driver.h"

inline void _send_msg(const char *msg)
{
  for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
  {
	  error_driver_write_c(msg[i]);
  }
  error_driver_write_c('\n');
}

void send_pgm(const prog_char *msg)
{
	uint8_t  myByte;
        myByte = pgm_read_byte(msg);
	for(int i = 1; myByte != '\0'; i++)
	{
		error_driver_write_c(myByte);
        myByte = pgm_read_byte(msg+i);
	}
}

#ifdef DEBUG

void error_init(void)
{
	error_driver_init();
}


void error_putc(const char c)
{
	error_driver_write_c(c);
}


void assert (bool condition, const char *msg) {
  if (condition) {
#ifdef SERIAL
    exit(1);
#else
    send_pgm(PSTR("ASS:"));
    _send_msg(msg);
#endif
  }
}

void info (const char *msg)
{
#ifdef SERIAL
	printf("[INFO]: %s\n", msg);
#else
    send_pgm(PSTR("INF:"));
    _send_msg(msg);
#endif
}

void warn (const char *msg)
{
#ifdef SERIAL
	printf("[WARN]: %s\n", msg);
#else
    send_pgm(PSTR("WARN:"));
    _send_msg(msg);
#endif
}

void debug (const char *msg)
{
#ifdef SERIAL
	printf("[DEBUG]: %s\n", msg);
#else
    send_pgm(PSTR("DBG:"));
    _send_msg(msg);
#endif
}

void error (const char *msg)
{
#ifdef SERIAL
	printf("[ERROR]: %s\n", msg);
#else
    send_pgm(PSTR("ERR:"));
    _send_msg(msg);
#endif
}
#endif

#ifdef DEBUG
void assert_pgm(bool condition, const prog_char *msg) {
	if (condition) {
		send_pgm(PSTR("ASS:"));
		send_pgm(msg);
		error_driver_write_c('\n');
	}
}

void info_pgm(const prog_char *msg)
{
	send_pgm(PSTR("INF:"));
	send_pgm(msg);
	error_driver_write_c('\n');
}

void warn_pgm(const prog_char *msg)
{
	send_pgm(PSTR("WARN:"));
	send_pgm(msg);
	error_driver_write_c('\n');
}


void error_pgm(const prog_char *msg)
{
	send_pgm(PSTR("ERR:"));
	send_pgm(msg);
	error_driver_write_c('\n');
}

void debug_pgm(const prog_char *msg)
{
	send_pgm(PSTR("DBG:"));
	send_pgm(msg);
	error_driver_write_c('\n');
}

void print_hex(uint8_t num)
{
	if (num<10)
		error_putc(num+48);
	else
	{
		switch (num)
		{
		case 10:
			error_putc('A'); break;
		case 11:
			error_putc('B'); break;
		case 12:
			error_putc('C'); break;
		case 13:
			error_putc('D'); break;
		case 14:
			error_putc('E'); break;
		case 15:
			error_putc('F'); break;
		default:
			error_putc('#'); break;
		}
	}
}

void byte_to_hex(uint8_t byte){
	uint8_t b2 = (byte & 0x0F);
	uint8_t b1 = ((byte & 0xF0)>>4);
	print_hex(b1);
	print_hex(b2);
}

#endif

