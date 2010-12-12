/**
 * the buttons driver avr version
 * @file buttons_avr.c
 * @author Stefan Profanter
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
