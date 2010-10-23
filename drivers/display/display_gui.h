#include "../../shared/common.h"
#include "display.h"

#ifndef __DISPLAY_GUI_H__
#define __DISPLAY_GUI_H__

#define BUTTON_INFO_BAR_HEIGHT 10;

enum display_gui_keys{
	display_gui_key_up = 1,
	display_gui_key_down,
	display_gui_key_left,
	display_gui_key_right,
	display_gui_key_a,
	display_gui_key_b,
	display_gui_key_none
};

enum display_gui_screens{
	display_gui_screen_none,
	display_gui_screen_bootup,
	display_gui_screen_menu
};

// Draw a progress bar
void display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress);
// Handle key press events
void display_gui_keypress(int key);
// Draw a menu
void display_gui_menu(const char* title, const char** options, const int default_option, void(*callback)(int));

#endif
