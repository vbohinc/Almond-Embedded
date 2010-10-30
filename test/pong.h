#ifndef __PONG_H__
#define __PONG_H__

#include "../shared/common.h"
#include "../drivers/display/display.h"
#include "../drivers/display/display_draw.h"
#include "../drivers/display/display_gui.h"
#include <math.h>

void pong(void);
void pong_update(void);
void pong_keypress(enum display_gui_keys key);
#endif