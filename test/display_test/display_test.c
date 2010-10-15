#ifndef X86
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

#include "../../drivers/display/display.h"
#include "../../drivers/display/display_draw.h"

#include "../../drivers/display/display_data.h"

#define UART_BAUD_RATE      9600ul

/*! Define that selects the Usart used in example. */
#define USART USARTC0

void display_test2(void)
{	
	bool value = true;
	for(int i = 0; i < 10;i++){
		for(uint8_t y = 0; y <= 64; y++){
			for(uint8_t x = 0; x <= 128; x++){
				display_set_pixel(x,y,value);
				display_flip();
			}
		}
		value = !value;
	}
}

#include <stdbool.h>
#include "SDL.h" // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>


int main(void)
{
	display_init();
	//display_test2();
//	display_draw_string(10,10,2,"Hallo");
	display_draw_image(0,0,(uint8_t*)image_logo);
//	display_draw_char(10,10,0,'#');

	SDL_Event keyevent;    //The SDL event that we will poll to get events.
	 

	while(1)
	{	
		while (SDL_PollEvent(&keyevent))   //Poll our SDL key event for any keystrokes.
		{
			switch(keyevent.type){
				case SDL_KEYDOWN:
					printf("Oh! Key press\n");
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
	}


}

