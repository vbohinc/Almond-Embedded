#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "common.h"

//Port J, Pin 0 bis 7

#define BUTTONS_PORT PORTJ

#define BUTTONS_PIN_UP 7
#define BUTTONS_PIN_DOWN 5
#define BUTTONS_PIN_LEFT 4
#define BUTTONS_PIN_RIGHT 6
#define BUTTONS_PIN_A 3
#define BUTTONS_PIN_B 2

/**
Initializes the Button-Port/Pins
*/
void button_init(void);

/**
Checks if a Button is pressed. Returns the values of enum display_gui_keys
*/
uint8_t button_pressed(void); 

#endif /*_BUTTONS_H__*/
