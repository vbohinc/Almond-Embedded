gcc -I/opt/local/include/SDL/ -I../../../shared/ -I../../../drivers/ -I../../../ -I../../../squirrel/ ../../../squirrel/gui/pong.c menu_test.c ../../../squirrel/gui/gui.c ../../../squirrel/gui/gui_data.c ../../../drivers/display/display.c ../../../drivers/platform/buttons.c ../../../squirrel/gui/gui_draw.c -std=c99 -DX86 `sdl-config --cflags --libs` -lSDL_gfx -lm -o test 
./test
