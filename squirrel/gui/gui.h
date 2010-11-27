/**
 * Highlevel Display Driver
 * @file gui.h
 * @author Matthias Schwab
 */
#include <common.h>
#include <display/display.h>
#include "gui_data.h"
#include "gui_draw.h"


#ifndef __DISPLAY_GUI_H__
#define __DISPLAY_GUI_H__

///button info bar height
#define BUTTON_INFO_BAR_HEIGHT 10
/// button info bar y position coordinate
#define BUTTON_INFO_BAR_TOP DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT
#define VERSION_MAJOR 0
#define VERSION_MINOR 1
///max menu option string lenght 
#define MENU_OPTION_LENGHT 20

///enum of all aviable keys
enum display_gui_keys
{
    display_gui_key_up = 1, ///< up key 
    display_gui_key_down, ///< down key
    display_gui_key_left, ///< left key
    display_gui_key_right, ///< right key
    display_gui_key_a, ///< a key
    display_gui_key_b, ///< b key
    display_gui_key_none ///< no key
};

///enum for alle aviable screen states
enum display_gui_screens
{
    display_gui_screen_none,  ///< no specific screen state
    display_gui_screen_bootup, ///< bootup screen is shown state
    display_gui_screen_menu, ///< menu is shown state
    display_gui_screen_fullscreenimage, ///< fullscreen image is shown state
    display_gui_screen_alert, ///< alter is shown state
    display_gui_screen_game, ///< game is running screen state
    display_gui_screen_about ///< about screen is running state
};


///the current screen state
extern enum display_gui_screens current_screen;
///indicates if action left has a function in current screen state
extern bool display_gui_left_available;
///indicates if action up has a function in current screen state
extern bool display_gui_up_available;
///indicates if action right has a function in current screen state
extern bool display_gui_right_available;
///indicates if action down has a function in current screen state
extern bool display_gui_down_available;
///indicates if a button has a function in current screen state
extern bool display_gui_a_available;
///indicates if a button has a function in current screen state
extern bool display_gui_b_available;
///is true if the button bar is visible at the bottom of the screen
extern bool display_gui_button_bar_visible;
///description string of current a button function 
extern const char* display_gui_a_function;
///description string of current b button function
extern const char* display_gui_b_function;

///current frame of bootup screen animation
uint8_t bootup_current_frame;

/// Refresh stuff before redrawing
void display_gui_refresh (void);

/**
* Handles key press events
* @param key that was pressed and will be handeled
*/
void display_gui_keypress (enum display_gui_keys key);


/**
* Draws a menu
* @param title of the menu
* @param options the menu contains 
* @param option_count the number of options
* @param default_option which is selected at first frame
* @param callback for the handling of key events regarding the selected option
*/
void display_gui_menu (const char* title, const char* options, uint8_t option_count, const uint8_t default_option, void (*callback) (int8_t));

/**
* Handles keypress events for the menu
* @param key that was pressed and will be handeled
*/
void display_gui_menu_keypress (enum display_gui_keys key);

/// Draws scrollbar if not enough space on screen
void display_gui_draw_scrollbar (void);

/** 
* Displays a full screen image
* @param image pointer uint8 array with pixel information
* @param callback for key pressed events
*/
void display_gui_image (const uint8_t* image, void (*callback) (void));

/** 
* Handles keypress events while displaying fullscreen image
* @param key that was pressed and will be handeled
*/
void display_gui_image_keypress (enum display_gui_keys key);


/**
* Sets Bootup Progress bar
* @param progress of the bootup process in percent from 0 to 100
*/
void display_gui_bootup_progress (uint16_t progress);

/** 
* Updates the bootup screen callback for BT discover
* @param percentage of current bootup procress from 0 to 100
*/
void display_gui_bootup_update_callback (uint16_t percentage);

/// Draws the bootup screen
void display_gui_bootup_screen (void);

/** 
* \deprecated { Draws a fake bootup line }
*/
void display_gui_bootup_line (const char* string, uint16_t wait);

/**
* Draws an alert with 2 options
* @param title of the alert
* @param message to be shown
* @param button1 description string
* @param button2 description string
* @param callback to handle button pressed events
*/
void display_gui_alert (const char* title, const char* message, const char* button1, const char* button2, void (*callback) (bool));

/**
* Handles keypress events for the alert
* @param key that is pressed and will be handeled
*/
void display_gui_alert_keypress (enum display_gui_keys key);

/**
* Displays the about screen with credits
* @param callback pointer which draws the about animation/screen
*/
void display_gui_about (void (*callback) (void));

/** 
* Handles keypress events for the credits
* @param key that was pressed in about screen
*/
void display_gui_keypress_about (enum display_gui_keys key);

/**
* Draws a progress bar
* @param x position coordinate
* @param y position coordinate
* @param width of the progress bar
* @param height of the progress bar
* @param progress state in percent to be drawn (from 0 to 100)
*/
void display_gui_progress_bar (uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress);

/// Draws button functions line
void display_gui_draw_button_bar (void);

/// Sleeps for a given amount of ms
void display_gui_sleep (uint16_t ms);

#endif
