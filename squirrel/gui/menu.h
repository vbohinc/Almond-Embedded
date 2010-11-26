/**
 * menu.h - Main and Sub-Menues
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 */


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
