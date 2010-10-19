#include "display.h"
#ifndef X86
#include <avr/io.h>
#include <util/delay.h>
#endif
// LINES
#define DISPLAY_PAGE_NUMBER 7  	//Starts with Page 0, here: 8 pages overall
#define DISPLAY_PAGE_INIT 0

// COLUMNS
// FIXME: Determine real col number
#define DISPLAY_COL_NUMBER 127	//Starts with column 0, here: 132 columns overall
#define DISPLAY_COL_HEIGHT 8
#define DISPLAY_COL_INIT 0 		//normal: 0, reverse: 4
#define DISPLAY_COL_OFFSET 1

// DISPLAY PORTS
#define DISPLAY_CS  6 // chip select
#define DISPLAY_RST 7 // reset
#define DISPLAY_RS  5 //Register Select input
#define DISPLAY_RD  3 //read
#define DISPLAY_WR  4 //write

bool transparency;
bool previous_transparency;


bool
display_get_transparency()
{
	return transparency;
}

void
display_set_transparency(bool value) 
{	
	transparency=value;
}

#ifndef X86
// Display command type
enum {
  DISPLAY_CMD,
  DISPLAY_DATA 
};
void display_test(void);

inline static void display_send (uint8_t value, bool data);

static uint8_t backbuffer[DISPLAY_BACKBUFFER_LINES][DISPLAY_BACKBUFFER_COLUMNS];

void 
display_init(void)
{
	transparency=false;
	
	//User system setup by external pins
	PORTF.DIR = 0xFF;
	set_bit(PORTH.DIR, DISPLAY_RS);
	set_bit(PORTH.DIR, DISPLAY_RST);
	set_bit(PORTH.DIR, DISPLAY_CS);
	set_bit(PORTH.DIR, DISPLAY_WR);
	set_bit(PORTH.DIR, DISPLAY_RD);

	set_bit(PORTH.OUT, DISPLAY_CS);


	clear_bit(PORTH.OUT, DISPLAY_RST);


	set_bit(PORTH.OUT, DISPLAY_RS);
	set_bit(PORTH.OUT, DISPLAY_WR);
	set_bit(PORTH.OUT, DISPLAY_RD);

	_delay_ms(1000);				//Waiting for stabilizing power
	set_bit(PORTH.OUT, DISPLAY_RST);

	display_send(0xA2, DISPLAY_CMD);	//LCD Bias Select

	//display_send(0xA1, DISPLAY_CMD);	//ADC SELECT
	//display_send(0xC8, DISPLAY_CMD);	//SHL Direction 0-64



	display_send(0x25, DISPLAY_CMD);	//Regulator resistor select
	display_send(0x81, DISPLAY_CMD);	//Set reference voltage mode        contrast
	display_send(0x30, DISPLAY_CMD);	//Set reference voltage register    contrast value
	display_send(0x00, DISPLAY_CMD);
	display_send(0x2F, DISPLAY_CMD);	//PowerControl
	display_send(0x40, DISPLAY_CMD);	//Initial Display line to 0
	display_send(0xA6, DISPLAY_CMD);	//REVERSE DISPLAY OFF
	display_send(0xAF, DISPLAY_CMD);	// Display ON

	display_send(0xA0, DISPLAY_CMD);	//ADC SELECT
	display_send(0xC0, DISPLAY_CMD);	//SHL Select

	display_send(0xB0, DISPLAY_CMD);	// SET PAGE ADDRESS 0
	display_send(0x10, DISPLAY_CMD);	//SET COLUMN ADDRESS MSB 0
	display_send(0x00, DISPLAY_CMD);	//SET COLUMN ADRESS LSB 0

/*
	display_send(0xA0, DISPLAY_CMD);	//ADC SELECT
	display_send(0xC0, DISPLAY_CMD);	//SHL Select
	display_send(0xA2, DISPLAY_CMD);	//LCD Bias Select
	display_send(0x25, DISPLAY_CMD);	//Regulator resistor select

	display_send(0x81, DISPLAY_CMD);	//Set reference voltage mode
	display_send(0x30, DISPLAY_CMD);	//Set reference voltage register

	display_send(0x2F, DISPLAY_CMD);	//PowerControl

	display_send(0x40, DISPLAY_CMD);	//Initial Display line to 0
	display_send(0xB0, DISPLAY_CMD);	// SET PAGE ADDRESS 0
	display_send(0x10, DISPLAY_CMD);	//SET COLUMN ADDRESS MSB 0
	display_send(0x00, DISPLAY_CMD);	//SET COLUMN ADRESS LSB 0
	display_send(0xA6, DISPLAY_CMD);	//REVERSE DISPLAY OFF
	display_send(0xAF, DISPLAY_CMD);	// Display ON
	display_send(0xA5, DISPLAY_CMD); // all pixel on
*/

   //display_send(0x10 + (5  >> 4), DISPLAY_DATA);
    //display_send(0x00 + (5  & 0x0F), DISPLAY_DATA);


    //display_send(0xB0 + 7, DISPLAY_DATA);




}

void
display_set_pixel(uint8_t x, uint8_t y, bool value)
{
	uint8_t page = y / 8;
	uint8_t col = x;
	uint8_t bit_index = 7 - (y % 8);
	
	if(x >= DISPLAY_BACKBUFFER_COLUMNS) x = DISPLAY_BACKBUFFER_COLUMNS - 1;
	if(y >= 64) x = 63;
	
	if(value)
		// Black pixel
		backbuffer[page][col] |= 1<<bit_index;
	else if(!transparency)
		// White pixel
		backbuffer[page][col] &= ~(1<<bit_index);
}

inline static void 
display_send (uint8_t value, bool data)
{	
	if(data==DISPLAY_DATA){
		set_bit(PORTH.OUT, DISPLAY_RS);		
	}else{
		clear_bit(PORTH.OUT, DISPLAY_RS);
	}
	// FIXME: Check interface
	clear_bit(PORTH.OUT, DISPLAY_CS);
	clear_bit(PORTH.OUT, DISPLAY_WR);
	set_bit(PORTH.OUT, DISPLAY_RD);
	/*value = (value & 0x55) << 1 | ((value >> 1) & 0x55); 
	value = (value & 0x33) << 2 | ((value >> 2) & 0x33); 
	PORTF.OUT = (value & 0x0F) << 4 | ((value >> 4) & 0x0F); */
	//PORTF.OUT = value;	
	PORTF.OUT = ((value&0x01)<<7)|((value&0x02)<<5)|((value&0x04)<<3)|
			((value&0x08)<<1)|((value&0x10)>>1)|((value&0x20)>>3)|
			((value&0x40)>>5)|((value&0x80)>>7);
	set_bit(PORTH.OUT, DISPLAY_WR);
	set_bit(PORTH.OUT, DISPLAY_CS);
}

inline static void 
display_set_col(uint8_t col)
{
	display_send(0x10 + (col >> 4), DISPLAY_CMD);		// MSB
	display_send(0x00 + (col & 0x0F), DISPLAY_CMD);	// LSB
}

inline static void 
display_set_page(uint8_t page)
{
	//Top down instead of bottom up
	uint8_t inverted_page = DISPLAY_PAGE_NUMBER - page;
	display_send(0xB0 + inverted_page, DISPLAY_CMD);
}

void
display_flip(void)
{
	for(uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++){
		display_set_page(page);
		display_set_col(0);
		for(uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++){
			display_send (backbuffer[page][col], DISPLAY_DATA);
		}
	}
}

void
display_clear(void)
{
	for(uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++){
		display_set_page(page);
		display_set_col(0);
		for(uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++){
			display_send (0, DISPLAY_DATA);
		}
	}
}

#else
#include <stdbool.h>
#include "SDL.h" // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

 
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320
 
SDL_Surface *screen; //This pointer will reference the backbuffer
SDL_Surface *tux;
SDL_Surface *sur;

int
InitVideo() {
  Uint32 flags = SDL_HWSURFACE|SDL_DOUBLEBUF;
  // Load SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return false;
  }
  atexit(SDL_Quit); // Clean it up nicely :)
 
  screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32, flags);//flags);
  if (!screen) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    return false;
  }
  return true;
}

void
set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	if (x>=128 || y>=64)
	{
		//printf("Coorinate out of range: %d|%d\n",x,y);
		return;
	}

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    
    
    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
} 

void
DrawImage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha) {
  
  SDL_Rect src, dst;
  
  src.x = sx;  src.y = sy;  src.w = sw;  src.h = sh;
  dst.x = dx;  dst.y = dy;  dst.w = src.w;  dst.h = src.h;
  
  SDL_SetAlpha(srcimg, SDL_SRCALPHA, alpha);
  SDL_BlitSurface(srcimg, &src, dstimg, &dst);
}

void
display_init(void) {
  transparency = false;
  int res = 0; //Results
  if (InitVideo() == false) exit(-1);
  
	SDL_WM_SetCaption("Awesome Almond Display", NULL);


  tux = SDL_CreateRGBSurface(SDL_HWSURFACE, 128, 64, 32,0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  for(int x=0; x<128;x++) {
  	for(int y=0; y<64;y++){
  		set_pixel(tux,x,y,0xffffffff);
  	}
  }
}

void
display_set_pixel (uint8_t x, uint8_t y, bool value) {
  if(value)
    set_pixel(tux,x,y,0x000000ff); //Black
  else if(!transparency)
    set_pixel(tux,x,y,0xffffffff); //White
}


void
display_flip (void) {
  sur = zoomSurface(tux, 5, 5, SMOOTHING_OFF);
  
  DrawImage(sur, 0,0, sur->w, sur->h, screen, 0, 0, 128);
  SDL_Flip(screen); //Refresh the screen
  SDL_FreeSurface(sur);
}
// Clear display
void
display_clear(void) {
  for(int x=0; x<128;x++) {
  	for(int y=0; y<64;y++){
  		set_pixel(tux,x,y,0xffffffff);
  	}
  }
}
#endif


