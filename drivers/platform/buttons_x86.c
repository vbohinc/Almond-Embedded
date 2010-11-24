/**
 * the buttons driver avr version
 * @file buttons_x86.c
 * @author Stefan Profanter
 */

#include <SDL_events.h>


void button_init (void)
{
    
}

uint8_t button_pressed (void)
{
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
					case SDLK_RETURN:
					case SDLK_SPACE:
						printf("A");
						button = display_gui_key_a;
						break;
					case SDLK_s:
					case SDLK_BACKSPACE:
						printf("B");
						button = display_gui_key_b;
						break;
					case SDLK_x:
						printf("(X) SDL quit escape key");
						exit(0);
						break;
					default:
						printf("Unknown %d", keyevent.key.keysym.sym);
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
	return button;
}
