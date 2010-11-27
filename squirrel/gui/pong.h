/**
 * Pong game for squirrel 
 * @file pong.h
 */
#ifndef __PONG_H__
#define __PONG_H__

#include <common.h>
#include <display/display.h>
#include "gui_draw.h"
#include "gui.h"
#include <math.h>

/**
* Inizialize pong and set callback function
* @param callback for handling back button function to return to menu
*/
void pong(void (*callback) (void));

/// update routine to get input and draw next game state
void pong_update(void);

/**
* key event handler
* @param key pressed key to handle
*/
void pong_keypress(enum display_gui_keys key);
#endif
