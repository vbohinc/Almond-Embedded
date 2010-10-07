#include "display_gui.h"
#include "display_draw.h"

void
display_gui_progress_bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress)
{
	uint8_t bar_width = (width / 100) * progress;
	display_draw_rect(x ,y ,x + width, y + height, false);
	display_draw_rect(x + 2 ,y + 2 ,x + width - 4, y + height - 4, true);
}