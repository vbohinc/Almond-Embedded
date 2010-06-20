 /**
 * Error handling functions
 */

#include <stdbool.h>
#include "ftdi.h"

#include <avr/eeprom.h>
#include "string_pool.h"

char error_builder[255];
 
void assert (bool condition, const char *msg) {
  if (condition) {
#ifdef SERIAL
    exit(1);
#else
    FTDISend('A');
    FTDISend('S');
    FTDISend('S');
    FTDISend(':');
    uint8_t i = 0;
    while (msg[i]!=0 && i<255)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
  }
}

void info (const char *msg)
{
#ifdef SERIAL
	printf("[INFO]: %s\n", msg);
#else
    FTDISend('I');
    FTDISend('N');
    FTDISend('F');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void warn(const char *msg)
{
#ifdef SERIAL
	printf("[WARN]: %s\n", msg);
#else
    FTDISend('W');
    FTDISend('R');
    FTDISend('N');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void error(const char *msg)
{
#ifdef SERIAL
	printf("[ERROR]: %s\n", msg);
#else
    FTDISend('E');
    FTDISend('R');
    FTDISend('R');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void debug(const char *msg)
{
#ifdef SERIAL
	printf("[DEBUG]: %s\n", msg);
#else
    FTDISend('D');
    FTDISend('B');
    FTDISend('G');
    FTDISend(':');
    for (uint8_t i=0; i<255 && msg[i]!='\0'; i++)
        FTDISend(msg[i]);
    FTDISend('\n');
#endif
	return;
}

void send_eeprom(const uint8_t *msg)
{
	uint8_t  myByte = 1;

	for (uint8_t i=0; myByte != '\0'; i++)
	{
		 myByte = eeprom_read_byte(msg+i);
		 if (myByte != '\0')
			 FTDISend(myByte);
	}
}


void assert_eeprom(bool condition, const uint8_t *msg) {
	if (condition) {
		send_eeprom(str_error_assert);
		send_eeprom(msg);
		FTDISend('\n');
	}
}

void info_eeprom(const uint8_t *msg)
{
	send_eeprom(str_error_info);
	send_eeprom(msg);
	FTDISend('\n');
}

void warn_eeprom(const uint8_t *msg)
{
	send_eeprom(str_error_warn);
	send_eeprom(msg);
	FTDISend('\n');
}

void error_eeprom(const uint8_t *msg)
{
	send_eeprom(str_error_error);
	send_eeprom(msg);
	FTDISend('\n');
}

void debug_eeprom(const uint8_t *msg)
{
	send_eeprom(str_error_debug);
	send_eeprom(msg);
	FTDISend('\n');
}
