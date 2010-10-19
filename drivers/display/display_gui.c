#include "display_gui.h"
#include "display_draw.h"

int current_screen = display_gui_screen_none;
bool display_gui_left_available = true;
bool display_gui_top_available = true;
bool display_gui_right_available = true;
bool display_gui_down_available = true;
bool display_gui_a_available = true;
bool display_gui_b_available = true;
char display_gui_a_function[10];
char display_gui_b_function[10];

void
display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress)
{
	if (progress > 100) progress = 100;
	uint8_t bar_width = (width / 100) * progress;
	display_draw_rect(x ,y ,x + width, y + height, false); //Border
	display_draw_rect(x + 2 ,y + 2 ,x + bar_width - 2, y + height - 2, true); //Bar
}

void
display_gui_keypress(int key)
{
	
}

// SELECTION MENU

const char** gui_menu_current_options;
int gui_menu_selection;
void(*gui_menu_callback)(int) = NULL;

void
display_gui_menu(const char* title, const char** options, const int default_option, void(*callback)(int))
{
	gui_menu_current_options = options;
	gui_menu_selection = default_option;
	gui_menu_callback = callback;
	
	display_draw_rect(0, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, false);
	display_draw_string(0, 0, 0, title);
	
	uint8_t option_index = 0;
	uint8_t current_y = DISPLAY_FONT0_HEIGHT + 2;
	while(options[option_index] != NULL){
		if(option_index == gui_menu_selection){
			display_draw_rect(2, current_y, DISPLAY_WIDTH - 3, current_y + DISPLAY_FONT0_HEIGHT, true);
			display_inverted = 1;
		}
		display_transparency = true;
		display_draw_string(3, current_y, 0, options[option_index]);
		current_y += DISPLAY_FONT0_HEIGHT + 1;
		display_inverted = 0;
		option_index++;
	}
}