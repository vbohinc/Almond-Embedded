#ifndef __PONG_H__
#define __PONG_H__

#include <common.h>
#include <display/display.h>
#include "gui_draw.h"
#include "gui.h"
#include <math.h>

void pong(void (*callback) (void));
void pong_update(void);
void pong_keypress(enum display_gui_keys key);
#endif
