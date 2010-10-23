#include "display_gui.h"
#include "display_draw.h"

uint8_t current_screen = display_gui_screen_none;
bool display_gui_left_available = true;
bool display_gui_up_available = true;
bool display_gui_right_available = true;
bool display_gui_down_available = true;
bool display_gui_a_available = true;
bool display_gui_b_available = true;
const char* display_gui_a_function;
const char* display_gui_b_function;

void
display_gui_keypress(enum display_gui_keys key)
{
	// Dispatch key press to currently active screen
	switch(current_screen){
		case display_gui_screen_none:
			// Omit key press
			break;
		case display_gui_screen_bootup:
			break;
		case display_gui_screen_menu: 
			display_gui_menu_keypress(key);
			break;
		default:
			break;
	}
}

void 
display_gui_refresh(void)
{
	display_gui_draw_button_bar();
}

// SELECTION MENU ========================================================

const char* gui_menu_title;					// Menu title
const char** gui_menu_current_options;		// Array of strings of option titles
int8_t gui_menu_selection;					// Index of currently selected object
uint8_t gui_menu_option_count;				// Total number of available options
void(*gui_menu_callback)(int8_t) = NULL;	// Callback function for menu selection or cancellation

void
display_gui_menu(const char* title, const char** options, const uint8_t default_option, void(*callback)(int8_t))
{
	current_screen = display_gui_screen_menu;
	gui_menu_current_options = options;
	gui_menu_selection = default_option;
	gui_menu_callback = callback;
	gui_menu_title = title;
	display_gui_a_function = "Select";
	display_gui_b_function = "Cancel";
	display_gui_left_available = false;
	display_gui_up_available = true;
	display_gui_right_available = false;
	display_gui_down_available = true;
	
	display_clear();
	display_draw_rect(0, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, false);
	display_draw_string(0, 0, 0, title);
	
	uint8_t option_index = 0;
	uint8_t current_y = DISPLAY_FONT0_HEIGHT + 2;
	while(options[option_index] != NULL){
		display_inverted = option_index != gui_menu_selection;
		display_draw_rect(2, current_y, DISPLAY_WIDTH - 3, current_y + DISPLAY_FONT0_HEIGHT, true);	
		display_transparency = true;
		display_inverted = !display_inverted;
		display_draw_string(3, current_y, 0, options[option_index]);
		display_transparency = false;
		current_y += DISPLAY_FONT0_HEIGHT + 1;
		option_index++;
		display_inverted = false;
	}
	gui_menu_option_count = option_index;
}

void
display_gui_menu_keypress(uint8_t key)
{
	switch(key){
		case display_gui_key_up:
			gui_menu_selection--;
			break;
		case display_gui_key_down:
			gui_menu_selection++;
			break;
		case display_gui_key_a:
			gui_menu_callback(gui_menu_selection);
			break;
		case display_gui_key_b:
			gui_menu_callback(-1);
		default:
			break;
	}
	// Wrap selection around
	if(gui_menu_selection < 0) gui_menu_selection = gui_menu_option_count - 1;
	if(gui_menu_selection > gui_menu_option_count - 1) gui_menu_selection = 0;
	// Redraw menu
	display_gui_menu(gui_menu_title, gui_menu_current_options, gui_menu_selection, gui_menu_callback);
}

// UTILITIES =====================================================

void
display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress)
{
	if (progress > 100) progress = 100;
	uint8_t bar_width = (width / 100) * progress;
	display_draw_rect(x ,y ,x + width, y + height, false); //Border
	display_draw_rect(x + 2 ,y + 2 ,x + bar_width - 2, y + height - 2, true); //Bar
}

void
display_gui_draw_button_bar(void)
{
	// Arrows panel
	display_draw_rect(0, BUTTON_INFO_BAR_TOP, 29, DISPLAY_HEIGHT, true);
	display_set_pixel(0, BUTTON_INFO_BAR_TOP, false);
	display_set_pixel(29, BUTTON_INFO_BAR_TOP, false);
	// A-Button panel
	display_draw_rect(31, BUTTON_INFO_BAR_TOP, 31+48, DISPLAY_HEIGHT, true);
	display_set_pixel(31, BUTTON_INFO_BAR_TOP, false);
	display_set_pixel(31+48, BUTTON_INFO_BAR_TOP, false);
	// B-Button panel
	display_draw_rect(81, BUTTON_INFO_BAR_TOP, 127, DISPLAY_HEIGHT, true);
	display_set_pixel(81, BUTTON_INFO_BAR_TOP, false);
	display_set_pixel(127, BUTTON_INFO_BAR_TOP, false);
	// A/B Functions
	display_inverted = true;
	display_draw_string(32, BUTTON_INFO_BAR_TOP + 1, 0, display_gui_a_function);
	display_draw_string(82, BUTTON_INFO_BAR_TOP + 1, 0, display_gui_b_function);
	// Arrows
	uint8_t current_x = 0;
	if(display_gui_left_available){
		display_draw_image(current_x, BUTTON_INFO_BAR_TOP + 1, arrow_left);
		current_x += 8;
	}
	if(display_gui_right_available){
		display_draw_image(current_x, BUTTON_INFO_BAR_TOP + 1, arrow_right);
		current_x += 8;
	}
	if(display_gui_up_available){
		display_draw_image(current_x, BUTTON_INFO_BAR_TOP + 1, arrow_up);
		current_x += 8;
	}
	if(display_gui_down_available){
		display_draw_image(current_x, BUTTON_INFO_BAR_TOP + 1, arrow_down);
	}
	
	display_inverted = false;
}