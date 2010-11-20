/**
 * \brief the display driver sdl testing version
 * @file display_x86.c
 * @addtogroup display
 * @author Pascal Schnurr
 */
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

SDL_Surface *screen;
SDL_Surface *tux;
SDL_Surface *sur;

void set_pixel (SDL_Surface * surface, int x, int y, Uint32 pixel);
void DrawImage (SDL_Surface * srcimg, int sx, int sy, int sw, int sh,
                SDL_Surface * dstimg, int dx, int dy, int alpha);

void
exit_if_fail (bool condition)
{
    if (!condition)
        return;

    fprintf (stderr, "%s\n", SDL_GetError ());
    atexit (SDL_Quit);
}

void
display_init (void)
{
    int res = 0;            //Results

    Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;

    exit_if_fail (SDL_Init (SDL_INIT_VIDEO));
    SDL_WM_SetCaption ("Almond Squirrel Emulator", NULL);

    screen = SDL_SetVideoMode (WINDOW_WIDTH, WINDOW_HEIGHT, 32, flags);
    exit_if_fail (screen != NULL);

    tux =
        SDL_CreateRGBSurface (SDL_HWSURFACE, DISPLAY_WIDTH, DISPLAY_HEIGHT, 32,
                              0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    exit_if_fail (tux != NULL);

    display_clear ();
}

void
display_set_pixel (uint8_t x, uint8_t y, bool value)
{
    if (value ^ inverted)
        set_pixel (tux, x, y, 0x000000ff);  //Black
    else
        set_pixel (tux, x, y, 0xffffffff);  //White
}

void
display_flip (void)
{
    sur = zoomSurface (tux, 5, 5, SMOOTHING_OFF);
    DrawImage (sur, 0, 0, sur->w, sur->h, screen, 0, 0, 128);
    SDL_Flip (screen);
    SDL_FreeSurface (sur);
}

void
display_clear (void)
{
    for (int x = 0; x < 128; x++)
        for (int y = 0; y < 64; y++)
            set_pixel (tux, x, y, 0xffffffff);
}

void
set_pixel (SDL_Surface * surface, int x, int y, Uint32 pixel)
{
    if (x >= 128 || y >= 64)
        return;

    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *) p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        }
        else
        {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *) p = pixel;
        break;
    }
}

void
DrawImage (SDL_Surface * srcimg, int sx, int sy, int sw, int sh,
           SDL_Surface * dstimg, int dx, int dy, int alpha)
{
    SDL_Rect src, dst;

    src.x = sx;
    src.y = sy;
    src.w = sw;
    src.h = sh;
    dst.x = dx;
    dst.y = dy;
    dst.w = src.w;
    dst.h = src.h;

    SDL_SetAlpha (srcimg, SDL_SRCALPHA, alpha);
    SDL_BlitSurface (srcimg, &src, dstimg, &dst);
}
