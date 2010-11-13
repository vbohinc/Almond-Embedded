#include "buttons.h"


#include <gui/display_gui.h>
#include <util/delay.h>
 
/* Einfache Funktion zum Entprellen eines Tasters */
uint8_t debounce(uint8_t pin)
{
    if ( !( BUTTONS_PORT.IN  & (1 << pin) ) )
    {
        /* Pin wurde auf Masse gezogen, 100ms warten   */
        _delay_ms(25);   // Maximalwert des Parameters an _delay_ms 
        //_delay_ms(50);   // beachten, vgl. Dokumentation der avr-libc
        if ( !(BUTTONS_PORT.IN  & (1 << pin)) )
        {
            /* Anwender Zeit zum Loslassen des Tasters geben */
            _delay_ms(25);
            //_delay_ms(50); 
            return 1;
        }
    }
    return 0;
}
 


void button_init(void)
{
	PORTCFG.MPCMASK=0xFF;	// Maske für Pinkonfiguration, hier werden alle pins maskiert
	BUTTONS_PORT.PIN0CTRL=PORT_OPC_PULLUP_gc;	// Für die maskierten Pins wird der Pullup Widerstand aktiviert

	BUTTONS_PORT.DIR = 0x00; /*Hier werden alle Bits von PORT als Eingang gesetzt*/
	//BUTTONS_PORT.OUT = 0xFF; //Pullup widerstand setzen
}

uint8_t button_pressed(void)
{
	if (debounce(BUTTONS_PIN_UP))
		return display_gui_key_up;
	else if (debounce(BUTTONS_PIN_DOWN))
		return display_gui_key_down;
	else if (debounce(BUTTONS_PIN_LEFT))
		return display_gui_key_left;
	else if (debounce(BUTTONS_PIN_RIGHT))
		return display_gui_key_right;
	else if (debounce(BUTTONS_PIN_A))
		return display_gui_key_a;
	else if (debounce(BUTTONS_PIN_B))
		return display_gui_key_b;
	else 
		return display_gui_key_none;
} 

