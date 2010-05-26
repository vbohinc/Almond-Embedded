/**
* @file display.c
* @author parsch
* @date 2010/05/26
*
* The display API
*/

#include "display.h"



bool display_write_running_text(char* text) {
	int text_max_length = LINES * SYMBOLS;

	if (text !=null){
		int length = strlen(text);
		if (legnth <= text_max_length) {
			//TODO communication to display board
			#ifdef TEST
			printf(text);
			#endif
			return true;
		}

		text += text_max_length-length;

		//TODO communication to display board
		#ifdef TEST
		printf(text);
		#endif
		return true;
	}
	return false;
}


bool display_write_line(char* text, int line) {
	int text_max_length = SYMBOLS;

	if (text !=null){
		int length = strlen(text);
		if (legnth <= text_max_length) {
			//TODO communication to display board
			#ifdef TEST
				printf(text);
				printf("write to line ");
			#endif
			return true;
		}

		text += text_max_length-length;

		//TODO communication to display board
		#ifdef TEST
			printf(text);
			printf("write to line ");
		#endif
		return true;
	}
	return false;
}


bool display_write_symbol (char symbol, int line, int line_position) {
	if (symbol != null){
	#ifdef TEST
			//TODO communication to board
			printf(symbol);
			printf("write to line");
			pritnf("write to position");
	#endif
	}
	return false;
}
