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

#include "../../drivers/display/display.h"
#include "../../drivers/display/display_draw.h"
#include "../../drivers/display/display_gui.h"

#include "../../drivers/display/display_data.h"
#include "../pong.h"
#include <platform/buttons.h>

#define USART USARTC0



// Display test for X86
void
did_select_menu(int8_t option)
{
	printf("OPTION SELECTED: %i\n", option);
}

#ifndef X86
int main (void)
#else
int main (int argc, char *argv[])
#endif
{

#ifndef X86
	/* Behold the MAGIC Clock! */

	/* Internen 32Mhz Oszillator einschalten */
	OSC.CTRL = OSC_RC32MEN_bm;

	/* Warten bis Oszillator stabil ist */
	while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);

	/* System Clock selection */
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;

	/* DFLL ein (Auto Kalibrierung) */
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;


	set_bit(PORTC.DIR,4);
	clear_bit(PORTC.OUT,4);

	set_bit(PORTH.DIR,2);
	set_bit(PORTH.OUT,2);
#endif

	display_init();
	button_init();

	const char *options[] = {"Eins", "Zwei", "Drei", NULL};
 	display_gui_menu("Menue", options, 1, &did_select_menu);

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
		/*switch (button)
		{
			case display_gui_key_up:
				display_print("UP Pressed\n");
				break;
			case display_gui_key_down:
				display_print("DOWN Pressed\n"); break;
			case display_gui_key_left:
				display_print("LEFT Pressed\n"); break;
			case display_gui_key_right:
				display_print("RIGHT Pressed\n"); break;
			case display_gui_key_a:
				display_print("A Pressed\n"); break;
			case display_gui_key_b:
				display_print("B Pressed\n"); break;
			default:
				break;

		}*/
#endif

		if (button != display_gui_key_none)
		{
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
