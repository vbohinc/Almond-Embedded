#include "pong.h"
#include "../shared/common.h"
#include "../drivers/display/display.h"
#include "../drivers/display/display_draw.h"
#include <math.h>
#include <unistd.h>


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
static char left_score_string[3];
static uint8_t right_score;
static char right_score_string[3];

static void draw_rectangle(rectangle *rect);
static void move_pad(rectangle *pad, int8_t *input);
static void ball_reset(void);
static void draw_center_line(void); 

void pong() { 
	//left_score_string = "000";
	//right_score_string = "000";
	pad_left.top_left.x = 1;
	pad_left.top_left.y = 1;
	pad_left.bottom_right.x = 3;
	pad_left.bottom_right.y = 17;
	
	pad_right.top_left.x = 124;
	pad_right.top_left.y = 46;
	pad_right.bottom_right.x = 126;
	pad_right.bottom_right.y = 62;
	
	ball_reset();

	ball_x_speed = 3;
	ball_y_speed = 0;
	
	right_score = 0;
	left_score = 0;
	
	left_pad_input = 1;
	right_pad_input = -1;
	
	// display_clear()?
	display_clear();
	display_flip();
	
	draw_rectangle(&pad_left);
	draw_rectangle(&pad_right);
	draw_rectangle(&ball);
	draw_center_line();
	sprintf(left_score_string,"%d",left_score);
	sprintf(right_score_string,"%d",right_score);
	display_draw_string(28,1,2,left_score_string);
	display_draw_string(100,1,2,right_score_string);
	display_flip();
	
	while(1==1) {
		
		//get_input(); // FIXME!
		move_pad(&pad_left, &left_pad_input);
		move_pad(&pad_right, &right_pad_input);
		if (ball.top_left.x + ball_x_speed < 0)  { // FIXME!
			if (ball.top_left.y >= pad_left.top_left.y && ball.bottom_right.y <= pad_left.bottom_right.y ) {
				ball_x_speed++;
				ball_x_speed = -ball_x_speed;
				uint8_t dist_from_pad_centre = pad_left.bottom_right.y - ball.top_left.y;
				ball_y_speed += dist_from_pad_centre;
			} else {
				// Paddle missed. Game over.
				left_score++;
				ball_reset();
				//printf("Left scores: %d", left_score);
			}
		} else if (ball.bottom_right.x + ball_x_speed > 128) {
			//printf("Ball: Right edge test. %d %d %d %d vs. %d %d\n", ball.top_left.x, ball.top_left.y, ball.bottom_right.x, ball.bottom_right.y, pad_right.top_left.y, pad_right.bottom_right.y);
			if (ball.top_left.y >= pad_right.top_left.y && ball.bottom_right.y <= pad_right.bottom_right.y ) {
				ball_x_speed++;
				ball_x_speed = -ball_x_speed;
				uint8_t dist_from_pad_centre = pad_right.bottom_right.y - ball.top_left.y;
				ball_y_speed += dist_from_pad_centre;
			} else {
				// Paddle missed. Game over.
				right_score++;
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
		
		usleep(100);
		display_clear();
		draw_rectangle(&pad_left);
		draw_rectangle(&pad_right);
		draw_rectangle(&ball);
		draw_center_line();
		sprintf(left_score_string,"%d",left_score);
		sprintf(right_score_string,"%d",right_score);
		display_draw_string(28,1,2,left_score_string);
		display_draw_string(100,1,2,right_score_string);
		display_flip();
	}
}

static void ball_reset() {
	ball.top_left.x = 65;
	ball.top_left.y = 32;
	ball.bottom_right.x = 67;
	ball.bottom_right.y = 34;
	ball_x_speed = 1;
	ball_y_speed = 0;
}

static void draw_center_line() {
	for (int i = 0; i < 16; i += 2) {
		display_draw_line(66,i*4,66,(i*4)+4);
	}
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
