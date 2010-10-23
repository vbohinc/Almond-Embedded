#ifndef X86

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE      9600ul

#else

#include <stdbool.h>
#include <SDL.h> // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#endif

#include "../../../drivers/display/display.h"
#include "../../../drivers/display/display_draw.h"
#include "../../../drivers/display/display_gui.h"

#include "../../../drivers/display/display_data.h"
#include "../../pong.h"

#ifndef X86
#include <platform/buttons.h>
#endif

#define USART USARTC0

void test_menu(void);

void
test_alert_callback(bool option)
{
	test_menu();
}

// Display test for X86
void
test_menu_selected(int8_t option)
{
	switch (option)
	{
		case 0:
			display_gui_about(&test_menu); break;
		case 1:
			display_gui_image(testimg, &test_menu); break;
		case 2:
			pong(); break;
		case 3:
			display_gui_alert("Hey you!", "Do you like nuts?", "Yeah", "Nope", &test_alert_callback); break;
		default:
			test_menu(); break;
	}
}

void 
test_menu(void)
{
	// Main menu for the test
 	static const char *options[] = {"About", "Nipples", "Pong", "Alert", NULL};
 	display_gui_menu("Pick a test", options, 0, &test_menu_selected);
}

#ifndef X86
int main (void)
#else
int main (int argc, char *argv[])
#endif
{

#ifndef X86
	OSC.CTRL = OSC_RC32MEN_bm;						// Internen 32Mhz Oszillator einschalten
	while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);	// Warten bis Oszillator stabil ist
	CCP = CCP_IOREG_gc;								// System Clock selection
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;				// DFLL ein (Auto Kalibrierung)

	set_bit(PORTC.DIR,4);
	clear_bit(PORTC.OUT,4);


#endif

	display_init();		// Initialize display

#ifndef X86
	button_init();
#endif

	display_gui_bootup_screen();
	test_menu();

	//display_draw_image(0,0,(uint8_t*)tum_logo_f2);

	display_flip();
	while(true) 
	{
#ifdef X86
		uint8_t button = display_gui_key_none;
		SDL_Event keyevent;
		while (SDL_PollEvent(&keyevent)) {
			switch(keyevent.type){
				case SDL_KEYDOWN:
					// Handle key events
					printf("Key pressed: ");
     					switch(keyevent.key.keysym.sym){
						case SDLK_UP:
							printf("^");
							button = display_gui_key_up;
							break;
						case SDLK_DOWN:
							printf("v");
							button = display_gui_key_down;
							break;
						case SDLK_LEFT:
							printf("<");
							button = display_gui_key_left;
							break;
						case SDLK_RIGHT:
							printf(">");
							button = display_gui_key_right;
							break;
						case SDLK_a:
							printf("A");
							button = display_gui_key_a;
							break;
						case SDLK_s:
							printf("B");
							button = display_gui_key_b;
							break;
						case SDLK_x:
							printf("(X) SDL quit escape key");
							exit(0);
							break;
						default:
							break;
					}
					printf("\n");
					break;
				case SDL_QUIT:
					printf("Quitting...\n");
					exit(0);
					break;
			}
		}
#else
		uint8_t button = button_pressed();
#endif
		if (button != display_gui_key_none)	{
			display_gui_keypress(button);
		}
		
		display_gui_refresh();	// Refresh gui drawings
		display_flip();
#ifndef X86
		_delay_ms(1);
#else
		SDL_Delay(100);
#endif
	}
}
