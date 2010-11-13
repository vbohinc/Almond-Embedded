#include "../../shared/common.h"
#include "../display/display.h"
#include "display_data.h"

#ifndef __DISPLAY_GUI_H__
#define __DISPLAY_GUI_H__

#define BUTTON_INFO_BAR_HEIGHT 10
#define BUTTON_INFO_BAR_TOP DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT
#define VERSION_MAJOR 0
#define VERSION_MINOR 1

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
	display_gui_screen_menu,
	display_gui_screen_fullscreenimage,
	display_gui_screen_alert,
	display_gui_screen_game,
	display_gui_screen_about
};


extern enum display_gui_screens current_screen;
extern bool display_gui_left_available;
extern bool display_gui_up_available;
extern bool display_gui_right_available;
extern bool display_gui_down_available;
extern bool display_gui_a_available;
extern bool display_gui_b_available;
extern bool display_gui_button_bar_visible;
extern const char* display_gui_a_function;
extern const char* display_gui_b_function;



// Refresh stuff before redrawing
void display_gui_refresh(void);
// Handle key press events
void display_gui_keypress(enum display_gui_keys key);


// Draw a menu
void display_gui_menu(const char* title, const char** options, const uint8_t default_option, void(*callback)(int8_t));
// Handle keypress events for the menu
void display_gui_menu_keypress(enum display_gui_keys key);

// Display a full screen image
void display_gui_image(const uint8_t* image, void(*callback)(void));
// Handle keypress events while displaying fullscreen image
void display_gui_image_keypress(enum display_gui_keys key);

// Draw the bootup screen
void display_gui_bootup_screen(void);
// Draw a fake bootup line
void display_gui_bootup_line(const char* string, uint16_t wait);

// Draw an alert with 2 options
void display_gui_alert(const char* title, const char* message, const char* button1, const char* button2, void(*callback)(bool));
// Handle keypress events for the alert
void display_gui_alert_keypress(enum display_gui_keys key);

// Display the about screen with credits
void display_gui_about(void(*callback)(void));
// Handle keypress events for the credits
void display_gui_keypress_about(enum display_gui_keys key);

// Draw a progress bar
void display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress);
// Draw button functions line
void display_gui_draw_button_bar(void);
// Sleep for a given amount of ms
void display_gui_sleep(uint16_t ms);

#endif
