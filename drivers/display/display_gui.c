#include "display_gui.h"
#include "display_draw.h"
#include "../../test/pong.h"

#ifdef X86
#include <SDL.h>
#endif

enum display_gui_screens current_screen = display_gui_screen_none;
bool display_gui_left_available = true;
bool display_gui_up_available = true;
bool display_gui_right_available = true;
bool display_gui_down_available = true;
bool display_gui_a_available = true;
bool display_gui_b_available = true;
bool display_gui_button_bar_visible = true;
const char* display_gui_a_function;
const char* display_gui_b_function;

void
display_gui_keypress(enum display_gui_keys key)
{
	// Dispatch key press to currently active screen
	switch(current_screen){
		case display_gui_screen_none:
			/* Omit key press */ break;
		case display_gui_screen_bootup:
			break;
		case display_gui_screen_menu: 
			display_gui_menu_keypress(key);	break;
		case display_gui_screen_fullscreenimage:
			display_gui_image_keypress(key); break;
		case display_gui_screen_alert:
			display_gui_alert_keypress(key); break;
		case display_gui_screen_game:
			pong_keypress(key); break;
		default:
			break;
	}
}

void 
display_gui_refresh(void)
{
	switch(current_screen) {
		case display_gui_screen_game:
			pong_update(); break;
		default:
			if(display_gui_button_bar_visible) display_gui_draw_button_bar();
	}
	
}

// SELECTION MENU ===============================================================================================

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
	display_gui_button_bar_visible = true;
	
	display_clear();
	display_draw_rect(0, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 2, false);
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
display_gui_menu_keypress(enum display_gui_keys key)
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
	if (key != display_gui_key_a && key != display_gui_key_b)
		display_gui_menu(gui_menu_title, gui_menu_current_options, gui_menu_selection, gui_menu_callback);
}

// BOOTUP SCREEN ======================================================================================

void
display_gui_bootup_screen(void)
{
	display_gui_button_bar_visible = false;
	display_draw_rect(0,0, DISPLAY_WIDTH, DISPLAY_HEIGHT, true);
	display_inverted = true;
	display_gui_bootup_line("ALMOND Squirrel v0.1 prealpha\n", 500);
	display_gui_bootup_line("Initializing storage\n", 200);
	display_gui_bootup_line("Detecting tits...\n", 400);
	display_gui_bootup_line("Destroying 'pong'...\n", 100);
	display_gui_bootup_line("Freaking out...\n", 600);
	display_inverted = false;
	display_clear();
	
	display_draw_string(40, DISPLAY_HEIGHT - 15, 1, "ALMOND");
	const uint8_t *almond_logo_frames[] = {almond_logo_f1, almond_logo_f2, almond_logo_f3, almond_logo_f4, NULL};
	display_draw_animated_image(39, 0, almond_logo_frames, 2, 300);
}

void
display_gui_bootup_line(const char* string, uint16_t wait)
{
	display_print(string);
	display_flip();
	display_gui_sleep(wait);
}

// FULLSCREEN IMAGE ===================================================================================

void(*gui_image_callback)(void) = NULL;	// Callback function for image dismissal

void
display_gui_image(const uint8_t* image, void(*callback)(void))
{
	current_screen = display_gui_screen_fullscreenimage;
	gui_image_callback = callback;
	display_gui_a_function = "Dismiss";
	display_gui_b_function = "";
	display_gui_left_available = false;
	display_gui_up_available = false;
	display_gui_right_available = false;
	display_gui_down_available = false;
	display_gui_button_bar_visible = true;
	
	display_clear();
	display_draw_image(0, 0, image);
}

void
display_gui_image_keypress(enum display_gui_keys key)
{
	if(key == display_gui_key_a) gui_image_callback();
}

// ALTERT =============================================================================================

void(*gui_alert_callback)(bool);

void
display_gui_alert(const char* title, const char* message, const char* button1, const char* button2, void(*callback)(bool))
{
	display_inverted = true;
	display_draw_rect(20, 20, DISPLAY_WIDTH - 20, DISPLAY_HEIGHT - 40, true);
	display_inverted = false;
	display_draw_rect(20, 20, DISPLAY_WIDTH - 20, DISPLAY_HEIGHT - 40, false);
}

void
display_gui_alert_keypress(enum display_gui_keys key)
{
	
}

// UTILITIES ==========================================================================================

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
	// Clear background
	display_inverted = true;
	display_draw_rect(0, BUTTON_INFO_BAR_TOP - 1, DISPLAY_WIDTH, DISPLAY_HEIGHT - 1, true);
	display_inverted = false;
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


void
display_gui_sleep(uint16_t ms)
{
	#ifdef X86
	SDL_Delay(ms);
	#else
	_delay_ms(ms);
	#endif
}