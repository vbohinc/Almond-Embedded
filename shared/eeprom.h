#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <avr/eeprom.h>

#define ESTR(s) (__extension__({static char __c[] EEMEM = (s); &__c[0];}))







#endif
