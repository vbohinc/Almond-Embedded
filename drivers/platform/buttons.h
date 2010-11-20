/**
 * button.h - Driver for the Buttons on XMega
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 */
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include "../../shared/common.h"

//Port J, Pin 0 bis 7

#define BUTTONS_PORT PORTJ

#define BUTTONS_PIN_UP 6
#define BUTTONS_PIN_DOWN 5
#define BUTTONS_PIN_LEFT 7
#define BUTTONS_PIN_RIGHT 4
#define BUTTONS_PIN_A 3
#define BUTTONS_PIN_B 2

/**
Initializes the Button-Port/Pins
*/
void button_init (void);

/**
Checks if a Button is pressed. Returns the values of enum display_gui_keys
*/
uint8_t button_pressed (void);

#endif /*_BUTTONS_H__*/
