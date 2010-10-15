#include <pong.h>

typedef struct {
	uint8_t x,
	uint8_t y
} coordinate;

typedef struct {
	coordinate top_left,
	coordinate bottom_right
} rectangle;


static rectangle pad_left;
static rectangle pad_right;
static rectangle ball;

static int16_t ball_x_speed;
static int16_t ball_y_speed;

void pong() { 
	pad_left.top_left.x = 1;
	pad_left.top_left.y = 1;
	pad_left.bottom_right.x = 3;
	pad_left.bottom_right.y = 17;
	
	pad_right.top_left.x = 60;
	pad_right.top_left.y = 114;
	pad_right.bottom_right.x = 130;
	pad_right.bottom_right.y = 62;
	
	ball.top_left.x = 65;
	ball.top_left.y = 32;
	ball.bottom_right.x = 67;
	ball.bottom_right.y = 34;
	
	ball_x_speed = 1;
	ball_y_speed = 0;
	
	// display_clear()?
	display_clean();
	display_flip();
	
	while(1==1) {
		get_input(); // FIXME!
		move_pad(&pad_left, input_l);
		move_pad(&pad_right, input_r);
		if (ball.top_left.x + ball_x_speed < 2)  { // FIXME!
			if (ball.top_left.y >= pad_left.top_left.y || ball.bottom_right.y <= pad_left.bottom_right.y ) {
				ball_x_speed++;
				uint8_t dist_from_pad_centre = pad_left.bottom_right.y - ball.top_left.y;
				ball_y_speed += dist_from_pad_centre;
			} else {
				// Paddle missed. Game over.
				break;
			}
		} else if (ball.bottom_right.x + ball_x_speed > 130) {
			if (ball.top_left.y >= pad_left.top_left.y || ball.bottom_right.y <= pad_left.bottom_right.y ) {
				ball_x_speed++;
				uint8_t dist_from_pad_centre = pad_left.bottom_right.y - ball.top_left.y;
				ball_y_speed += dist_from_pad_centre;
			} else {
				// Paddle missed. Game over.
				break;
			}
		} else if (ball.top_left.y + ball_y_speed < 0) {
			uint8_t reflected_y = abs(ball_y_speed - ball.top_left.y);
			ball.top_left.y = reflected_y;
			ball.bottom_right.y = reflected_y + 2;
			ball.top_left.x += ball_x_speed;
			ball.bottom_right.x += ball_x_speed;
			ball_y_speed = -ball_y_speed;
		} else if (ball.bottom_right.y + ball_y_speed > 64) {
			uint8_t reflected_y = 64 - (ball_y_speed - (64-ball.bottom_right.y));
			ball.top_left.y = reflected_y+2;
			ball.bottom_right.y = reflected_y;
			ball.top_left.x += ball_x_speed;
			ball.bottom_right.x += ball_x_speed;
			ball_y_speed = -ball_y_speed;
		}
	}
}

static void move_pad(rectangle *pad, int8_t input) {
	if (pad->top_left.y + input < 0) {
		pad->top_left.y = 0;
		pad->bottom_right.y = 16;
	} else if (pad->bottom_right.y > 64) {
		pad->bottom_right.y = 63;
		pad->top_left.y = 47;
	} else {
		pad->top_left.y += input;
		pad->bottom_right.y += input;
	}
}
