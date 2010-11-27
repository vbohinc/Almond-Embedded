/**
 * Main and Sub-Menues
 * @file menu.h
 * @author Stefan Profanter
 */

#include <stdbool.h>

///enum for menu return
enum menu_return {
	//!Returned if squirrel can continue with whatever (Main menu shown)
	MEN_CONTINUE,
	//!User want's a new search of NUT
	MEN_NEW_SEARCH
};

/**
 * Squirrel calls this function to update menu and handle user inputs
 * @return Value to indicate what squirrel can do next
 */
enum menu_return menu_update(void);

/**
 * Shows a message and blocks the screen, if Backend is connected with squirrel
 **/
void menu_slave_connected(bool isConnected);
