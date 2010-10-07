#include "display_draw.h"
#include "display.h"
#include "display_data.h"

static void
display_draw_char(uint8_t x, uint8_t y, uint8_t font_size, char* asciiIndex)
{
	uint8_t PROGMEM font;
	if(font_size <= 0) font = font_0;
	if(font_size == 1) font = font_1;
	if(font_size >= 2) font = font_2;
	uint8_t char_width = font[0];
	uint8_t char_height = font[1];
	uint8_t bit_index = 0;
	uint8_t byte_index = 0;
	uint8_t char_index = asciiIndex - FONT_CHAR_ASCII_OFFSET;
	// Fall back to '?' for characters out of range
	char_index = 31 if(char_index <= FONT_CHAR_MIN || char_index >= FONT_CHAR_MAX);
	
	for(uint8_t cy = y; cy <= y + char_height; cy++){
		for(uint8_t cx = x; cx <= x + char_width; cx++){
			display_set_pixel(x, y, font[char_index][byte_index]>>(7-bit_index) & 1);
			bit_index++;
			if(bit_index >= 8){
				bit_index = 0; 
				byte_index++;
			}
		}		
	}
}

static void
display_draw_rect(uint8_t topLeftX, uint8_t topLeftY, uint8_t bottomRightX, uint8_t bottomRightY)
{
	
}

void 
display_draw_line(uint8_t xstart ,uint8_t ystart ,uint8_t xend ,uint8_t yend)
{
	uint8_t x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

	// Calculate distances
	dx = xend - xstart;
	dy = yend - ystart;

	// Determine sign (movement direction)
	incx = sgn(dx);
	incy = sgn(dy);
	if(dx<0) dx = -dx;
	if(dy<0) dy = -dy;

	// Determine faster direction
	if (dx>dy){
		// X is fast direction
		pdx=incx; pdy=0;	// Parallel movement
		ddx=incx; ddy=incy; // Diagonal movement
		es =dy;	el =dx;		// Fast/slow error correction
	}else{
		// Y is fast direction
		pdx=0;	 pdy=incy; 	// Parallel movement
		ddx=incx; ddy=incy; // Diagonal movement
		es =dx;	el =dy;		// Fast/slow error correction
	}

	x = xstart;
	y = ystart;
	err = el/2;
	display_set_pixel(x,y,1);

	for(t=0; t<el; ++t){
		err -= es; 
		if(err<0){
			 err += el;
			 x += ddx;
			 y += ddy;
		}else{
			 x += pdx;
			 y += pdy;
		}
		display_set_pixel(x,y,1);
	}
}

// Signum function, helper function for line drawing
uint8_t
sgn(int x)
{
	return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}