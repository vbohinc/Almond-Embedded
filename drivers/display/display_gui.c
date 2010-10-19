#include "display_gui.h"
#include "display_draw.h"

void
display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress)
{
	if (progress > 100)
		progress = 100;
	uint8_t bar_width = (width / 100) * progress;
	display_draw_rect(x ,y ,x + width, y + height, false);//Border
	display_draw_rect(x + 2 ,y + 2 ,x + bar_width - 2, y + height - 2, true); //Bar
}

void
display_gui_keypress()