ALMOND_DIR='../..'

clang -I/opt/local/include/SDL/ -I$ALMOND_DIR/shared/ -I$ALMOND_DIR/drivers/ -I$ALMOND_DIR/protocols -I$ALMOND_DIR/ -I$ALMOND_DIR/squirrel/ $ALMOND_DIR/squirrel/gui/pong.c main.c $ALMOND_DIR/squirrel/gui/gui.c $ALMOND_DIR/squirrel/gui/menu.c $ALMOND_DIR/squirrel/gui/gui_data.c $ALMOND_DIR/drivers/display/display.c $ALMOND_DIR/drivers/platform/buttons.c $ALMOND_DIR/drivers/rtc/rtc.c $ALMOND_DIR/squirrel/gui/gui_draw.c $ALMOND_DIR/shared/time_conversion.c -std=c99 -DX86 `sdl-config --cflags --libs` -lSDL_gfx -lm -o test 
./test
