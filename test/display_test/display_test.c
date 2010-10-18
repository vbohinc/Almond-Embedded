#ifndef X86
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#endif

#include "../../drivers/display/display.h"
#include "../../drivers/display/display_draw.h"
#include "../../drivers/display/display_gui.h"

#include "../../drivers/display/display_data.h"
#include "../pong.h"

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
#include <SDL.h> // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>


int main (int argc, char *argv[])
{
	display_init();
	
	//display_test2();
	//display_draw_string(10,10,2,"Hallo");
	//display_draw_image(0,0,(uint8_t*)image_logo);
	
	//display_draw_line(12,12,20,60);

	//display_draw_char(10,10,0,'#');
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
	display_set_transparency(true);

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

