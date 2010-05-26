// display.h squirrel display api

#define TEST 1

#define LINES 4 //Number of lines presentable in the display
#define SYMBOLS 20 //Number of symbols per line

#ifdef TEST
#include <stdio.h>
#endif

#include <avr/io.h>
#include <string.h>
#include <stdbool.h>

/**
* Writes running text on the screen
* @param text pointer to the text char array
* @return fail or success
*
* Clears the screen and writes the whole text on the screen.
* If the text length is longer than the screen capability, it only displays the end of the text.
*/
bool display_write_running_text(char* text);

/**
* Writes a single line on the screen
* @param text pointer to the text char array
* @param line number of the line to be overwritten
* @return fail or success
*
* Clears a certain line and writes a running text in it
*/
bool display_write_line(char* text, int line);

/**
* Writes a single char on the screen
* @param symbol the char to be written
* @param line number of the line
* @param position position of the char inside the line
* @return fail or success
*
* Writes a single char to a certain position.
*/
bool display_write_symbol (char symbol, int line, int line_position);



