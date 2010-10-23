#include "../../shared/common.h"
#include "display.h"

#ifndef __DISPLAY_GUI_H__
#define __DISPLAY_GUI_H__

#define BUTTON_INFO_BAR_HEIGHT 10
#define BUTTON_INFO_BAR_TOP DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT

enum display_gui_keys{
	display_gui_key_up,
	display_gui_key_down,
	display_gui_key_left,
	display_gui_key_right,
	display_gui_key_a,
	display_gui_key_b
};

enum display_gui_screens{
	display_gui_screen_none,
	display_gui_screen_bootup,
	display_gui_screen_menu
};

// Refresh stuff before redrawing
void display_gui_refresh(void);
// Handle key press events
void display_gui_keypress(enum display_gui_keys key);


// Draw a menu
void display_gui_menu(const char* title, const char** options, const uint8_t default_option, void(*callback)(int8_t));
// Handle keypress events for the menu
void display_gui_menu_keypress(uint8_t key);

// Draw a progress bar
void display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress);
// Draw button functions line
void display_gui_draw_button_bar(void);

#endif