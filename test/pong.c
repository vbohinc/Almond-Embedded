#include "pong.h"
#include "../shared/common.h"
#include "../drivers/display/display.h"
#include "../drivers/display/display_draw.h"
#include "../drivers/display/display_gui.h"
#include <math.h>
//#include <unistd.h>

#ifdef X86
#include <SDL.h>
#else
#include <platform/buttons.h>
#endif

typedef struct {
	uint8_t x;
	uint8_t y;
} coordinate;

typedef struct {
	coordinate top_left;
	coordinate bottom_right;
} rectangle;

static rectangle pad_left;
static rectangle pad_right;
static rectangle ball;

static int16_t ball_x_speed;
static int16_t ball_y_speed;

static int8_t left_pad_input;
static int8_t right_pad_input;

static uint8_t left_score;
static char left_score_string[4];
static uint8_t right_score;
static char right_score_string[4];

static void draw_rectangle(rectangle *rect);
static void move_pad(rectangle *pad, int8_t *input);
static void ball_reset(void);
static void pad_reset(void);
static void draw_center_line(void);
static void draw_ui(void); 

void pong() { 
	
	ball_reset();
	pad_reset();
	
	right_score = 0;
	left_score = 0;
	
	left_pad_input = 0;
	right_pad_input = 0;
	
	draw_ui();
	
	while(1==1) {
		//get_input(); // FIXME!
		move_pad(&pad_left, &left_pad_input);
		move_pad(&pad_right, &right_pad_input);
		#ifndef X86
		left_pad_input = 1;
		right_pad_input = -1;
		#endif
		#ifdef X86
		left_pad_input = 0;
		right_pad_input = 0;
		#endif
		if (ball.top_left.x + ball_x_speed < 2)  { // FIXME!
			if (ball.top_left.y >= (pad_left.top_left.y-2) && ball.bottom_right.y <= (pad_left.bottom_right.y+2) ) {
				if (ball_x_speed == 1 || ball_x_speed == -1) break;
				ball_x_speed--;
				ball_x_speed = -ball_x_speed;
				
				int8_t dist_from_pad_centre = 8 + (ball.top_left.y - pad_left.bottom_right.y );
				printf("%d\n", dist_from_pad_centre);
				ball_y_speed = 
					dist_from_pad_centre > 0 ?
						(ball_y_speed < 0 ? (ball_y_speed-1) : (ball_y_speed+1))
					:
						(ball_y_speed < 0 ? -(ball_y_speed-1) : -(ball_y_speed+1));
			} else {
				// Paddle missed. Game over.
				right_score++;
				ball_reset();
				//printf("Left scores: %d", left_score);
			}
		} else if (ball.bottom_right.x + ball_x_speed > 126) {
			//printf("Ball: Right edge test. %d %d %d %d vs. %d %d\n", ball.top_left.x, ball.top_left.y, ball.bottom_right.x, ball.bottom_right.y, pad_right.top_left.y, pad_right.bottom_right.y);
			if (ball.top_left.y >= (pad_right.top_left.y-2) && ball.bottom_right.y <= (pad_right.bottom_right.y+2) ) {
				if (ball_x_speed == 1 || ball_x_speed == -1) break;
				ball_x_speed--;
				ball_x_speed = -ball_x_speed;
				int8_t dist_from_pad_centre = 8 + (ball.top_left.y - pad_right.bottom_right.y );
				printf("%d\n", dist_from_pad_centre);
				ball_y_speed = 
					dist_from_pad_centre > 0 ?
						(ball_y_speed < 0 ? (ball_y_speed-1) : (ball_y_speed+1))
					:
						(ball_y_speed < 0 ? -(ball_y_speed-1) : -(ball_y_speed+1));
				//dist_from_pad_centre > 0 ? (ball_y_speed < 0 ? -(ball_y_speed-1) : -(ball_y_speed+1)) : (ball_y_speed < 0 ? -(ball_y_speed+1) : -(ball_y_speed-1));
			} else {
				// Paddle missed. Game over.
				left_score++;
				ball_reset();
				//printf("Right scores: %d", right_score);
			}
		} else if (ball.top_left.y + ball_y_speed < 0) {
			uint8_t reflected_y = fabs((ball_y_speed - ball.top_left.y));
			ball.top_left.y = reflected_y;
			ball.bottom_right.y = reflected_y + 2;
			ball.top_left.x += ball_x_speed;
			ball.bottom_right.x += ball_x_speed;
			ball_y_speed = -ball_y_speed;
		} else if (ball.bottom_right.y + ball_y_speed > 64) {
			uint8_t reflected_y = (64 - ball_y_speed - (64-ball.bottom_right.y));
			ball.top_left.y = reflected_y-2;
			ball.bottom_right.y = reflected_y;
			ball.top_left.x += ball_x_speed;
			ball.bottom_right.x += ball_x_speed;
			ball_y_speed = -ball_y_speed;
		} else {
			ball.top_left.y += ball_y_speed;
			ball.bottom_right.y += ball_y_speed;
			ball.top_left.x += ball_x_speed;
			ball.bottom_right.x += ball_x_speed;
		}
		#ifdef X86
		SDL_Event keyevent;
		SDL_PollEvent(&keyevent);
		switch(keyevent.type){
			case SDL_KEYDOWN:
				switch(keyevent.key.keysym.sym){
					case SDLK_w:
						printf("Left Up pressed.\n");
						left_pad_input = -1;
						break;
					case SDLK_s:
						printf("Left Down pressed.\n");
						left_pad_input = 1;
						break;
					case SDLK_UP:
						printf("Right Up pressed.\n");
						right_pad_input = -1;
						break;
					case SDLK_DOWN:
						printf("Right Down pressed.\n");
						right_pad_input = 1;
						break;
					case SDLK_x:
						exit(0);
						break;
					default:
						left_pad_input = 0;
						right_pad_input = 0;
				}
		}
		SDL_Delay(10);
		#else
		uint8_t button = button_pressed();
		switch (button)
		{
			case display_gui_key_up:
				left_pad_input = -1;
				break;
			case display_gui_key_down:
				left_pad_input = 1; break;
			case display_gui_key_left:
				 break;
			case display_gui_key_right:
				 break;
			case display_gui_key_a:
				right_pad_input = 1; break;
			case display_gui_key_b:
				right_pad_input = -1; break;
			default:
				left_pad_input = 0;
				right_pad_input = 0;

		}
		_delay_ms(10);
		#endif
		#ifndef X86
		/*if (pad_left.top_left.y < 2) {
		left_pad_input = 1;
		} else if (pad_left.bottom_right.y > 62){
			left_pad_input = -1;
		} else {
			left_pad_input = 0;
		}
		
		if (pad_right.top_left.y < 2) {
		right_pad_input = 1;
		} else if (pad_right.bottom_right.y > 62){
			right_pad_input = -1;
		} else {
			right_pad_input = 0;
		}*/
		#endif
		
		draw_ui();
	
	}
}

static void ball_reset() {
	ball.top_left.x = 65;
	ball.top_left.y = 32;
	ball.bottom_right.x = 67;
	ball.bottom_right.y = 34;
	ball_x_speed = 3;
	ball_y_speed = 0;
}

static void draw_center_line() {
	for (int i = 0; i < 16; i += 2) {
		display_draw_line(63,i*4,63,(i*4)+4);
	}
}

static void draw_ui() {
	display_clear();
	draw_rectangle(&pad_left);
	draw_rectangle(&pad_right);
	draw_rectangle(&ball);
	draw_center_line();
	sprintf(left_score_string,"%d",left_score);
	sprintf(right_score_string,"%d",right_score);
	display_draw_string(25,1,2,left_score_string);
	display_draw_string(95,1,2,right_score_string);
	display_flip();
}

static void pad_reset() {
	pad_left.top_left.x = 1;
	pad_left.top_left.y = 1;
	pad_left.bottom_right.x = 3;
	pad_left.bottom_right.y = 17;
	
	pad_right.top_left.x = 124;
	pad_right.top_left.y = 46;
	pad_right.bottom_right.x = 126;
	pad_right.bottom_right.y = 62;
}

static void draw_rectangle(rectangle *rect) {
	display_draw_rect(rect->top_left.x, rect->top_left.y, rect->bottom_right.x, rect->bottom_right.y, 1);
}

static void move_pad(rectangle *pad, int8_t *input) {
	if (pad->top_left.y + *input < 0) {
		pad->top_left.y = 0;
		pad->bottom_right.y = 16;
		//printf("Hit top: %d, %d\n", pad->top_left.y, pad->bottom_right.y);
		*input = -(*input);
	} else if (pad->bottom_right.y + *input >= 63) {
		pad->bottom_right.y = 62;
		pad->top_left.y = 46;
		//printf("Hit bottom: %d, %d\n", pad->top_left.y, pad->bottom_right.y);
		*input = -(*input);
	} else {
		pad->top_left.y += *input;
		pad->bottom_right.y += *input;
		//printf("Moving: %d, %d\n", pad->top_left.y, pad->bottom_right.y);
	}
}
