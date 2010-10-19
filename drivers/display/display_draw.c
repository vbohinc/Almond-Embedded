#ifndef X86
#include <avr/pgmspace.h>
#endif


#include "display_draw.h"
#include "display.h"
#include "display_data.h"

#ifdef X86
#include <stdbool.h>
#include "SDL.h" // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#endif

// Text buffer
#define DISPLAY_TEXTBUFFER_WIDTH 21
#define DISPLAY_TEXTBUFFER_HEIGHT 8
#define DISPLAY_TEXTBUFFER_CHAR_WIDTH 6
#define DISPLAY_TEXTBUFFER_CHAR_HEIGHT 8

char    text_buffer [DISPLAY_TEXTBUFFER_HEIGHT][DISPLAY_TEXTBUFFER_WIDTH+1];
uint8_t text_buffer_column = 0;
uint8_t text_buffer_line = 0;

// Signum function, helper function for line drawing
static uint8_t
sgn(int x)
{
	return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

uint8_t display_get_font_value(uint8_t font_size, int idx, int byte)
{

	#ifndef X86
	if(font_size <= 0) 
		return pgm_read_byte(font_0[idx][byte]);
	if(font_size == 1)
		return pgm_read_byte(font_1[idx][byte]);
	if(font_size >= 2)
		return pgm_read_byte(font_2[idx][byte]);

	#else
	if(font_size <= 0) 
		return font_0[idx][byte];
	if(font_size == 1)
		return font_1[idx][byte];
	if(font_size >= 2)
		return font_2[idx][byte];
	#endif
}

void
display_draw_char(uint8_t x, uint8_t y, uint8_t font_size, char asciiIndex)
{
	bool old_trans = display_get_transparency();
	display_set_transparency(true);
	uint8_t char_width = display_get_font_value(font_size, 0, 0);
	uint8_t char_height = display_get_font_value(font_size, 0, 1);;
	uint8_t bit_index = 0;
	uint8_t byte_index = 0;
	uint8_t char_index = asciiIndex - FONT_CHAR_ASCII_OFFSET;
	// Fall back to '?' for characters out of range
	if(char_index < FONT_CHAR_MIN || char_index > FONT_CHAR_MAX)
	  char_index = 32;

	
	for(uint8_t cy = y; cy < y + char_height; cy++){
		for(uint8_t cx = x; cx < x + char_width; cx++){
			display_set_pixel(cx, cy, display_get_font_value(font_size,char_index,byte_index) >>(7-bit_index) & 1);
			bit_index++;
			if(bit_index >= 8){
				bit_index = 0; 
				byte_index++;
			}
		}
	}
	
	if(old_trans != true)
		display_set_transparency(false);
}

void
display_draw_string(uint8_t x, uint8_t y, uint8_t font_size, char* char_array)
{
	
	uint8_t char_width = display_get_font_value(font_size, 0, 0);
	uint8_t index = 0;
	uint8_t current_x = x;
	while(char_array[index] != '\0'){
		display_draw_char(current_x, y, font_size, char_array[index]);
		index++;
		current_x += char_width;
	}
}

void
display_draw_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool filled)
{
	if(!filled){
		display_draw_line(x1, y1, x2, y1);
		display_draw_line(x2, y1, x2, y2);
		display_draw_line(x1, y2, x2, y2);
		display_draw_line(x1, y1, x1, y2);
	}else{
		for(uint8_t y = y1; y <= y2; y++){
			for(uint8_t x = x1; x <= x2; x++){
				display_set_pixel(x, y, 1);
			}
		}
	}
}

void 
display_draw_line(uint8_t xstart ,uint8_t ystart ,uint8_t xend ,uint8_t yend)
{
	int16_t x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;

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

static void
display_textbuffer_shiftup(void){
	for(uint8_t line = 0; line < DISPLAY_TEXTBUFFER_HEIGHT; line++){
		for(uint8_t column = 0; column < DISPLAY_TEXTBUFFER_WIDTH; column++){
			if(line < DISPLAY_TEXTBUFFER_HEIGHT - 1){
				text_buffer[line][column] = text_buffer[line + 1][column];
			}else{
				text_buffer[line][column] = ' ';
			}
		}

	}
}

void 
display_print(char* char_array)
{
	uint32_t char_index = 0;
	while(char_array[char_index] != '\0'){
		if (char_array[char_index] != '\n')
		{
			text_buffer[text_buffer_line][text_buffer_column] = char_array[char_index];
			text_buffer_column++;
		}
		char_index++;
		if(text_buffer_column >= DISPLAY_TEXTBUFFER_WIDTH || char_array[char_index]=='\n'){
			text_buffer[text_buffer_line][text_buffer_column]='\0';
			text_buffer_column = 0;
			text_buffer_line++;
		}
		if(text_buffer_line >= DISPLAY_TEXTBUFFER_HEIGHT && char_array[char_index] != '\0'){
			// Bounced to bottom
			display_textbuffer_shiftup();
			text_buffer_line--;
		}
	}
	
	for(uint8_t line = 0; line < DISPLAY_TEXTBUFFER_HEIGHT; line++){
		display_draw_string(0, line * DISPLAY_TEXTBUFFER_HEIGHT, 0, text_buffer[line]);
	}
}

void
display_draw_image(int16_t topx, int16_t topy, const uint8_t* image_array){
	uint8_t image_width = image_array[0];
	uint8_t image_height = image_array[1];

	uint16_t byte_index = 2;
	uint8_t bit_index = 0;
	for(uint8_t y = 0; y < image_height; y++){
		for(uint8_t x = 0; x < image_width; x++){
			#ifndef X86
			display_set_pixel(topx + x, topy + y, pgm_read_byte(&image_array[byte_index]) >>(7-bit_index) & 1);
			#else
			display_set_pixel(topx + x, topy + y, image_array[byte_index] >>(7-bit_index) & 1);
			#endif
			bit_index++;
			if(bit_index >= 8){
				bit_index = 0; 
				byte_index++;
			}
		}
	}
	display_flip();
}


/*
Takes an array of image pointers and prints the given images in forward direction and then backwards round times
*/
void
display_animated_image(int8_t posX, int8_t posY, const uint8_t** image_pointer_array, uint8_t rounds, uint8_t sleep){
	
	for (uint8_t r = 0; r<rounds; r++)
	{
		uint8_t currImg = 0;
		int adder = 1;
		const uint8_t *p;
		do	
		{
			if (currImg >= 0 && image_pointer_array[currImg]!= NULL)
			{
				p = image_pointer_array[currImg];
				display_draw_image(posX, posY,p);
				display_flip();
				SDL_Delay(sleep);
			} else {
				adder *= -1;
			}
			currImg += adder;

		} while (currImg > 0);
	}
	display_draw_image(posX, posY,image_pointer_array[0]);
	display_flip();
}
