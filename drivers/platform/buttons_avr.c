/**
 * buttons_x86.c - the buttons driver windows version
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 */

void button_init (void)
{
    PORTCFG.MPCMASK = 0xFF; // Maske für Pinkonfiguration, hier werden alle pins maskiert
    BUTTONS_PORT.PIN0CTRL = PORT_OPC_PULLUP_gc; // Für die maskierten Pins wird der Pullup Widerstand aktiviert

    BUTTONS_PORT.DIR = 0x00; /*Hier werden alle Bits von PORT als Eingang gesetzt*/
    //BUTTONS_PORT.OUT = 0xFF; //Pullup widerstand setzen
}

/** \brief Button Debounce
 *
 * Debounces a Button-Press at the given pin
 */
uint8_t debounce (uint8_t pin)
{
    if (! (BUTTONS_PORT.IN  & (1 << pin)))
    {
	// Pin is on ground, wait some time
        _delay_ms (25);

        if (! (BUTTONS_PORT.IN  & (1 << pin)))
        {
		//Pin is still on ground
		//Give user time to release button
            _delay_ms (25);
            return 1;
        }
    }

    return 0;
}

uint8_t button_pressed (void)
{
    if (debounce (BUTTONS_PIN_UP))
        return display_gui_key_up;
    else
        if (debounce (BUTTONS_PIN_DOWN))
            return display_gui_key_down;
        else
            if (debounce (BUTTONS_PIN_LEFT))
                return display_gui_key_left;
            else
                if (debounce (BUTTONS_PIN_RIGHT))
                    return display_gui_key_right;
                else
                    if (debounce (BUTTONS_PIN_A))
                        return display_gui_key_a;
                    else
                        if (debounce (BUTTONS_PIN_B))
                            return display_gui_key_b;
                        else
                            return display_gui_key_none;
}
