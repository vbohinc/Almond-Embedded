/**
 * gui.c - Highlevel Display Driver
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Matthias Schwab
 */
#include "gui.h"
#include "gui_draw.h"
#include "pong.h"
#include <string.h>

#ifdef X86
#include <SDL.h>
#endif

#define MAX_MENU_ENTRIES 4

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

void display_gui_sleep (uint16_t ms);

void
display_gui_keypress (enum display_gui_keys key)
{
    // Dispatch key press to currently active screen
    switch (current_screen)
    {

        case display_gui_screen_none:
            /* Omit key press */
            break;

        case display_gui_screen_bootup:
            break;

        case display_gui_screen_menu:
            display_gui_menu_keypress (key);
            break;

        case display_gui_screen_fullscreenimage:
            display_gui_image_keypress (key);
            break;

        case display_gui_screen_alert:
            display_gui_alert_keypress (key);
            break;

        case display_gui_screen_game:
            pong_keypress (key);
            break;

        case display_gui_screen_about:
            display_gui_keypress_about (key);
            break;

        default:
            break;
    }
}

void
display_gui_refresh (void)
{
    switch (current_screen)
    {

        case display_gui_screen_game:
            pong_update();
            break;

        default:

            if (display_gui_button_bar_visible)
                display_gui_draw_button_bar();
    }

}

// SELECTION MENU ===============================================================================================

const char* gui_menu_title;                 // Menu title
const char* gui_menu_current_options;       // Array of strings of option titles
int8_t gui_menu_selection;                  // Index of currently selected object
uint8_t gui_menu_option_count;              // Total number of available options
uint8_t gui_menu_first_option_shown;        // Index of option which is currently shown at position 0 (used for scrolling)
void (*gui_menu_callback) (int8_t) = NULL;  // Callback function for menu selection or cancellation

void
display_gui_menu (const char* title, const char* options, uint8_t option_count, const uint8_t default_option, void (*callback) (int8_t))
{
    current_screen = display_gui_screen_menu;

    if (gui_menu_current_options != options)
        gui_menu_first_option_shown = 0;

    gui_menu_current_options = options;

    gui_menu_selection = default_option;

    gui_menu_option_count = option_count;

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

    display_draw_rect (0, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 2, false);

    display_draw_string (0, 0, 0, title);

    if (default_option < gui_menu_first_option_shown)
        gui_menu_first_option_shown = default_option;

    if (default_option >= gui_menu_first_option_shown + MAX_MENU_ENTRIES)
        gui_menu_first_option_shown = default_option - (MAX_MENU_ENTRIES - 1);

    uint8_t current_y = DISPLAY_FONT0_HEIGHT + 2;

    for (uint8_t option_index = gui_menu_first_option_shown;option_index < gui_menu_first_option_shown + 4;option_index++)
    {
        display_set_inverted (option_index != gui_menu_selection);
        display_draw_rect (2, current_y, DISPLAY_WIDTH - 3, current_y + DISPLAY_FONT0_HEIGHT, true);
        //display_transparency = true;
        display_set_inverted (!display_get_inverted());
        display_draw_string (3, current_y, 0, options + option_index*MENU_OPTION_LENGHT);
        //display_transparency = false;
        current_y += DISPLAY_FONT0_HEIGHT + 1;
        display_set_inverted (false);
    }

    display_gui_draw_scrollbar();
}

void
display_gui_draw_scrollbar()
{
    if (gui_menu_option_count <= MAX_MENU_ENTRIES)
        return;

    uint8_t width = 5;

    display_set_inverted (true);

    display_draw_rect (DISPLAY_WIDTH - width, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH, DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 2, true);

    display_set_inverted (false);

    display_draw_line (DISPLAY_WIDTH - width, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH - width, DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 2);

    display_set_inverted (true);

    display_draw_line (DISPLAY_WIDTH - width - 1, DISPLAY_FONT0_HEIGHT + 1, DISPLAY_WIDTH - width - 1, DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 3);

    display_set_inverted (false);


    double height = DISPLAY_HEIGHT - DISPLAY_FONT0_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 2;

    uint8_t bar_size = (uint8_t) (MAX_MENU_ENTRIES / (double) gui_menu_option_count * height);

    if (bar_size < 2)
        bar_size = 2;

    uint8_t pos = (uint8_t) (gui_menu_first_option_shown / (double) gui_menu_option_count * height);

    display_draw_rect (DISPLAY_WIDTH - width + 2, DISPLAY_FONT0_HEIGHT + pos, DISPLAY_WIDTH, DISPLAY_FONT0_HEIGHT + 1 + pos + bar_size, true);

}

void
display_gui_menu_keypress (enum display_gui_keys key)
{
    switch (key)
    {

        case display_gui_key_up:
            gui_menu_selection--;
            break;

        case display_gui_key_down:
            gui_menu_selection++;
            break;

        case display_gui_key_a:
            gui_menu_callback (gui_menu_selection);
            break;

        case display_gui_key_b:
            gui_menu_callback (-1);

        default:
            break;
    }

    // Wrap selection around

    if (gui_menu_selection < 0)
        gui_menu_selection = gui_menu_option_count - 1;

    if (gui_menu_selection > gui_menu_option_count - 1)
        gui_menu_selection = 0;

    // Redraw menu
    if (key != display_gui_key_a && key != display_gui_key_b)
        display_gui_menu (gui_menu_title, gui_menu_current_options, gui_menu_option_count, gui_menu_selection, gui_menu_callback);
}

// BOOTUP SCREEN ======================================================================================

void
display_gui_bootup_progress (uint16_t progress)
{
    display_gui_progress_bar (14, DISPLAY_HEIGHT - 12, 100, 8, progress);
}

void
display_gui_bootup_screen (void)
{
    bootup_current_frame = 0;
    display_gui_button_bar_visible = false;

    display_clear();
    const uint8_t *almond_logo_frames[] = { almond_logo_f1, almond_logo_f2, almond_logo_f3, almond_logo_f4, NULL};
    bootup_current_frame = display_draw_animation_frame (39, 0, almond_logo_frames, bootup_current_frame);
    display_gui_bootup_progress (0);
    display_flip();

#ifdef X86
    //FIXME DELETE ME ONLY FOR TESTING
    /* display_gui_sleep(1000);
     display_gui_bootup_update_callback(10);
     display_gui_sleep(1000);
     display_gui_bootup_update_callback(30);
     display_gui_sleep(1000);
     display_gui_bootup_update_callback(45);
     display_gui_sleep(1000);
     display_gui_bootup_update_callback(65);
     display_gui_sleep(1000);
     display_gui_bootup_update_callback(83);
     display_gui_sleep(1000);
     display_gui_bootup_update_callback(100);*/
#endif

}


void
display_gui_bootup_update_callback (uint16_t percentage)
{
    const uint8_t *almond_logo_frames[] = { almond_logo_f1, almond_logo_f2, almond_logo_f3, almond_logo_f4, NULL};
    display_clear();
    bootup_current_frame = display_draw_animation_frame (39, 0, almond_logo_frames, bootup_current_frame);
    display_gui_bootup_progress (percentage);
    display_flip();
}

void
display_gui_bootup_line (const char* string, uint16_t wait)
{
    display_print (string);
    display_flip();
    display_gui_sleep (wait);
}

// FULLSCREEN IMAGE ===================================================================================

void (*gui_image_callback) (void) = NULL; // Callback function for image dismissal

void
display_gui_image (const uint8_t* image, void (*callback) (void))
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
    display_draw_image (0, 0, image);
}

void
display_gui_image_keypress (enum display_gui_keys key)
{
    if (key == display_gui_key_a)
        gui_image_callback();
}

// ALTERT =============================================================================================

void (*gui_alert_callback) (bool);

void
display_gui_alert (const char* title, const char* message, const char* button1, const char* button2, void (*callback) (bool))
{
    current_screen = display_gui_screen_alert;
    gui_alert_callback = callback;
    display_gui_a_function = button1;
    display_gui_b_function = button2;
    display_gui_left_available = false;
    display_gui_up_available = false;
    display_gui_right_available = false;
    display_gui_down_available = false;
    display_gui_button_bar_visible = true;

    display_clear();
    display_draw_rect (0, DISPLAY_FONT0_HEIGHT, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - BUTTON_INFO_BAR_HEIGHT - 2, false);

    display_draw_string (0, 0, 0, title);
    display_draw_string (3, DISPLAY_FONT0_HEIGHT + 3, 0, message);
}

void
display_gui_alert_keypress (enum display_gui_keys key)
{
    if (key == display_gui_key_a)
        gui_alert_callback (true);

    if (key == display_gui_key_b)
        gui_alert_callback (false);
}

// CREDITS ============================================================================================

#define CREDITS_CHARACTER_DELAY 60
#define CREDITS_DELAY 2000

void (*gui_about_callback) (void) = NULL;

void
display_gui_about (void (*callback) (void))
{
    gui_about_callback = callback;
    current_screen = display_gui_screen_about;
    display_gui_button_bar_visible = false;
    display_clear();
    display_draw_image (0, 2, image_logo);
    display_draw_string_delayed (62, 5, 2, "ALMOND", 400);
    display_draw_string_delayed (62, 22, 0, "Autonomous", 100);
    display_draw_string_delayed (62, 30, 0, "Logging And", 100);
    display_draw_string_delayed (62, 38, 0, "Management", 100);
    display_draw_string_delayed (62, 46, 0, "Of Network", 100);
    display_draw_string_delayed (62, 54, 0, "Devices", 100);
    display_gui_sleep (2000);
    display_clear();
    display_draw_string_delayed (30, 30, 0, "starring", 100);
    display_gui_sleep (1000);

    static const uint8_t *images[] = {team_salomon, team_linus, team_maximilian, team_sean, team_stefan, team_matthias, team_pascal, team_thomas, team_christian, NULL};
    static const char *names[] = {"Salomon", "Linus", "Maxi", "Sean", "Stefan", "Matthias", "Pascal", "Thomas", "Christian", NULL};
    static const char *assignments[] = {"Protocols", "Drivers, Nut", "Hardware", "Pong, SD", "Gui, Bluetooth", "Backend, Gui", "Gui EMU, Docu", "Documentation", "Backend", NULL};

    uint8_t current_person = 0;

    while (images[current_person] != NULL)
    {
        display_clear();
        display_draw_image (0, 0, images[current_person]);
        display_draw_string_delayed (45, 20, 2, names[current_person], CREDITS_CHARACTER_DELAY);
        display_draw_string_delayed (45, 38, 0, assignments[current_person], CREDITS_CHARACTER_DELAY);
	
        display_gui_sleep (CREDITS_DELAY);
        current_person++;
    }

    gui_about_callback();
}

void
display_gui_keypress_about (enum display_gui_keys key)
{

}

// UTILITIES ==========================================================================================

void
display_gui_progress_bar (uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t progress)
{
    if (progress > 100)
        progress = 100;

    uint8_t bar_width = (width / 100) * progress;

    display_draw_rect (x , y , x + width, y + height, false); //Border

    display_draw_rect (x + 2 , y + 2 , x + bar_width - 2, y + height - 2, true); //Bar
}

void
display_gui_draw_button_bar (void)
{
    // Clear background
    display_set_inverted (true);
    display_draw_rect (0, BUTTON_INFO_BAR_TOP - 1, DISPLAY_WIDTH, DISPLAY_HEIGHT - 1, true);
    display_set_inverted (false);
    // Arrows panel

    if (display_gui_right_available || display_gui_left_available || display_gui_up_available || display_gui_down_available)
    {
        display_draw_rect (0, BUTTON_INFO_BAR_TOP, 29, DISPLAY_HEIGHT, true);
        display_set_pixel (0, BUTTON_INFO_BAR_TOP, false);
        display_set_pixel (29, BUTTON_INFO_BAR_TOP, false);
    }

    // A-Button panel

    if (strcmp (display_gui_a_function, ""))
    {
        display_draw_rect (31, BUTTON_INFO_BAR_TOP, 31 + 48, DISPLAY_HEIGHT, true);
        display_set_pixel (31, BUTTON_INFO_BAR_TOP, false);
        display_set_pixel (31 + 48, BUTTON_INFO_BAR_TOP, false);
    }

    // B-Button panel

    if (strcmp (display_gui_b_function, ""))
    {
        display_draw_rect (81, BUTTON_INFO_BAR_TOP, 127, DISPLAY_HEIGHT, true);
        display_set_pixel (81, BUTTON_INFO_BAR_TOP, false);
        display_set_pixel (127, BUTTON_INFO_BAR_TOP, false);
    }

    // A/B Functions
    display_set_inverted (true);

    display_draw_string (32, BUTTON_INFO_BAR_TOP + 1, 0, display_gui_a_function);

    display_draw_string (82, BUTTON_INFO_BAR_TOP + 1, 0, display_gui_b_function);

    // Arrows
    uint8_t current_x = 0;

    if (display_gui_left_available)
    {
        display_draw_image (current_x, BUTTON_INFO_BAR_TOP + 1, arrow_left);
        current_x += 8;
    }

    if (display_gui_right_available)
    {
        display_draw_image (current_x, BUTTON_INFO_BAR_TOP + 1, arrow_right);
        current_x += 8;
    }

    if (display_gui_up_available)
    {
        display_draw_image (current_x, BUTTON_INFO_BAR_TOP + 1, arrow_up);
        current_x += 8;
    }

    if (display_gui_down_available)
    {
        display_draw_image (current_x, BUTTON_INFO_BAR_TOP + 1, arrow_down);
    }

    display_set_inverted (false);
}


void
display_gui_sleep (uint16_t ms)
{
#ifdef X86
    SDL_Delay (ms);
#else
    _delay_ms (ms);
#endif
}
