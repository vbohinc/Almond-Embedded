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
#include "../../drivers/display/buttons.h"

#define USART USARTC0

#ifndef X86
// Display test for AVR
int main (void)
{
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

	display_init();
	button_init();
//	display_test2();
//	_delay_ms(1000);
//	display_flip();
	display_draw_image(0,0,(uint8_t*)tum_logo_f2);
	display_flip();
	display_set_pixel(0,0,true);
	display_set_pixel(0,1,true);
	display_set_pixel(0,2,true);
	display_set_pixel(0,3,true);
	display_set_pixel(0,4,true);
	display_set_pixel(0,5,true);
	//display_set_pixel(0,6,true);
//	display_draw_string(0,0,0,"hallo Sebi!");
//	display_flip();*/
//	display_draw_string(0,0,0,"1111111111111111111111111111111111");
	display_flip();
	while(true) 
	{
/*	display_gui_key_up = 1,
	display_gui_key_down,
	display_gui_key_left,
	display_gui_key_right,
	display_gui_key_a,
	display_gui_key_b,
	display_gui_key_none*/
		switch (button_pressed())
		{
			case display_gui_key_up:
				display_print("UP Pressed"); break;
			case display_gui_key_down:
				display_print("DOWN Pressed"); break;
			case display_gui_key_left:
				display_print("LEFT Pressed"); break;
			case display_gui_key_right:
				display_print("RIGHT Pressed"); break;
			case display_gui_key_a:
				display_print("A Pressed"); break;
			case display_gui_key_b:
				display_print("B Pressed"); break;
			default:
				break;

		}
		_delay_ms(1);
	}
}
#else
// Display test for X86
void
did_select_menu(int8_t option)
{
	printf("OPTION SELECTED: %i\n", option);
}


int main (int argc, char *argv[])
{
	display_init();
	const char *options[] = {"Eins", "Zwei", "Drei", NULL};
 	display_gui_menu("Menue", options, 1, &did_select_menu);
//	display_funhouse();
	
	SDL_Event keyevent;

	while(1) {
		while (SDL_PollEvent(&keyevent)) {
			switch(keyevent.type){
				case SDL_KEYDOWN:
					// Handle key events
					printf("Key pressed: ");
     					switch(keyevent.key.keysym.sym){
						case SDLK_UP:
							printf("^");
							display_gui_keypress(display_gui_key_up);
							break;
						case SDLK_DOWN:
							printf("v");
							display_gui_keypress(display_gui_key_down);
							break;
						case SDLK_LEFT:
							printf("<");
							display_gui_keypress(display_gui_key_left);
							break;
						case SDLK_RIGHT:
							printf(">");
							display_gui_keypress(display_gui_key_right);
							break;
						case SDLK_a:
							printf("A");
							display_gui_keypress(display_gui_key_a);
							break;
						case SDLK_s:
							printf("B");
							display_gui_keypress(display_gui_key_b);
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
		display_gui_refresh();	// Refresh gui drawings
		display_flip();			// Flip backbuffer
		SDL_Delay(100);
	}
}
#endif
