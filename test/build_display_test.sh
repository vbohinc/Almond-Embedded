gcc display_test.c -DX86 -c -std=c99
gcc ../drivers/display/display.c -std=c99 -DX86 `sdl-config --cflags` -c
gcc ../drivers/display/display_draw.c -std=c99 -DX86 `sdl-config --cflags` -c


gcc display_test.o display.o display_draw.o `sdl-config --libs` -lSDL_gfx -o test 

