gcc -I/opt/local/include/SDL/ -I../shared/ ./pong.c ./squirrel/display/display_test.c ../drivers/gui/display_gui.c ../drivers/gui/display_data.c ../drivers/display/display.c ../drivers/gui/display_draw.c -std=c99 -DX86 `sdl-config --cflags --libs` -lSDL_gfx -lm -o test 
./test
