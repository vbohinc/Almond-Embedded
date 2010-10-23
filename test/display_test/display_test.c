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

#define USART USARTC0

#ifndef X86
// Display test for AVR
int main ()
{
	
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
							printf(">");
							display_gui_keypress(display_gui_key_left);
							break;
						case SDLK_RIGHT:
							printf("<");
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


/*
void
display_funhouse()
{
	for (int8_t i=-64; i<0; i+=1)
	{
		display_draw_image(0,i,tum_logo_f1);
		if (i<-32)
			i+=1;
		if (i<-15)
			i+=1;
		SDL_Delay(10);
	}

	display_draw_image(0,0,(uint8_t*)testimg);
	SDL_Delay(25);

	const uint8_t *imgArr[] = {tum_logo_f1,tum_logo_f2,tum_logo_f3,tum_logo_f4,tum_logo_f5,tum_logo_f6,tum_logo_f7,tum_logo_f8, NULL};

	display_animated_image(0,0,imgArr,3,100);

	SDL_Event keyevent;    //The SDL event that we will poll to get events.
	 

	char arr[2000];
		int line = 1;
	int prog = 0;

	while(1)
	{	
		while (SDL_PollEvent(&keyevent))   //Poll our SDL key event for any keystrokes.
		{
			switch(keyevent.type){
				case SDL_KEYDOWN:
					printf("Oh! Key press\n");

					//SEE: http://www.libsdl.org/docs/html/sdlkey.html

     					switch(keyevent.key.keysym.sym){
						case SDLK_a:
							display_draw_char(10,10,0,'a');
							printf("a pressed\n");
							break;
						case SDLK_HASH:
							display_draw_char(10,10,0,'#');
							printf("# pressed\n");
							break;
						case SDLK_t:
							display_draw_image(0,0,(uint8_t*)testimg);
							printf("SuperKey 't' pressed\n");
							break;
						case SDLK_l:
							display_draw_image(0,0,(uint8_t*)image_logo);
							printf("Logo 't' pressed\n");
							break;
						case SDLK_b:
							display_gui_progress_bar(10,5,100,20,prog);
							prog+=10;
							printf("ProgressBar pressed\n");
							break;

						case SDLK_p:
							sprintf(arr,"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum pharetra arcu quis augue pulvinar adipiscing. Quisque ac est tortor. Duis dapibus fringilla nunc a aliquet. Praesent nulla quam, dignissim at fringilla et, suscipit iaculis turpis. Integer fringilla, leo id luctus feugiat, lacus nunc congue torto%d",line);
							line++;
							display_print(arr);
							printf("text pressed\n");
							break;
						case SDLK_q:
							
								sprintf(arr, "Boom!\n");
								line++;
								display_print(arr);
								printf("'P' pressed\n");
								pong();
								break;
							
						case SDLK_x:
							exit(0);
							break;
						default:
							break;
					}
					break;

					
				case SDL_QUIT:
					exit(0);
					break;
			}
		}

		display_flip();
		SDL_Delay(100);
	}
}

*/
#endif
