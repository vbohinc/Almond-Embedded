gcc -I/opt/local/include/SDL/ ./display_test/display_test.c ../drivers/display/display_gui.c ../drivers/display/display_data.c ../drivers/display/display.c ../drivers/display/display_draw.c -std=c99 -DX86 `sdl-config --cflags --libs` -lSDL_gfx -lm -o test 
./test
