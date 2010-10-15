#include <stdbool.h>
#include "SDL.h" // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#include "../drivers/display/display_draw.h"
 
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define true 1
#define false 0 //You might have to declare True and False.
#define COLORKEY 255, 0, 255 //Your Transparent colour
 
SDL_Surface *screen; //This pointer will reference the backbuffer
 
int InitVideo() {
  Uint32 flags = SDL_DOUBLEBUF;
  // Load SDL
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    return false;
  }
  atexit(SDL_Quit); // Clean it up nicely :)
 
  // fullscreen can be toggled at run time :) any you might want to change the flags with params?
  //set the main screen to SCREEN_WIDTHxSCREEN_HEIGHT with a colour depth of 16:
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, flags);
  if (!screen) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    return false;
  }
  return true;
}

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32 *)target_pixel = pixel;
} 

void DrawImage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Surface *dstimg, int dx, int dy, int alpha) {
  if ((!srcimg) || (alpha == 0)) return; //If theres no image, or its 100% transparent.
  SDL_Rect src, dst;
  
  src.x = sx;  src.y = sy;  src.w = sw;  src.h = sh;
  dst.x = dx;  dst.y = dy;  dst.w = src.w;  dst.h = src.h;
  //if (alpha != 255) SDL_SetAlpha(srcimg, SDL_SRCALPHA, alpha);
  // - This is incorrect, if alpha is 10, then set to 255, the image alpha will still be 10.
  SDL_SetAlpha(srcimg, SDL_SRCALPHA, alpha);
  SDL_BlitSurface(srcimg, &src, dstimg, &dst);
}

SDL_Surface* tux;

void display_init(void) {
  int res = 0; //Results
  if (InitVideo() == false) close(-1);
  
  tux = SDL_CreateRGBSurface(SDL_SWSURFACE, 128, 64, 32,0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  for(int x=0; x<128;x++) {
  	for(int y=0; y<64;y++){
  		set_pixel(tux,x,y,0xffffffff);
  	}
  }
}

void display_set_pixel (uint8_t x, uint8_t y, bool value) {   
  if(value)
  set_pixel(tux,x,y,0x00000000);
  else
  set_pixel(tux,x,y,0xffffffff);
  //SDL_ScaleSurface(tux, 512,256);
  SDL_Surface *sur = zoomSurface(tux, 5, 5, SMOOTHING_OFF);
  DrawImage(sur, 0,0, sur->w, sur->h, screen, 0, 0, 128);
}

void display_flip (void) {
  SDL_Flip(screen); //Refresh the screen
}
// Clear display
void display_clear(void) {
  for(int x=0; x<128;x++) {
  	for(int y=0; y<64;y++){
  		set_pixel(tux,x,y,0xffffffff);
  	}
  }
}

int main() {
  display_init();
  
  display_set_pixel(10,10,true);
  display_flip();
  
  SDL_Delay(3500); //Wait a bit :)
 
  //cleanup
  //SDL_FreeSurface(tux);
  return 0;
}
//Remember the blank line at the end! gcc always bugs me about it :) 
