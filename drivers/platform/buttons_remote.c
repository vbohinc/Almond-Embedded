/**
 * buttons_avr.c - the buttons driver avr version
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 */

enum display_gui_keys  curpressed = display_gui_key_none;

void button_init (void)
{
}

enum display_gui_keys button_pressed (void)
{
	enum display_gui_keys temp = curpressed;
	curpressed = display_gui_key_none;
	return temp;
}

void set_gui_key_pressed(enum display_gui_keys button)
{
	curpressed = button;
}
