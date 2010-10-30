#include <rtc/rtc.h>
#include <display/display.h>
#include <twi/twi.h>
#include <string.h>

int main(int argc, char *argv) {
	twi_init();
	display_init();
	display_clear();
	display_flip();
	debug_pgm(PSTR("Start\n"));
	time_t my_time = {59,59,23,31,12,100,6};
	set_time (&my_time);
	debug_pgm(PSTR("wayne!\n"));
	_delay_ms(1000);
	time_t their_time = get_time();
	char ausgabe[40];
	sprintf(ausgabe, "%d %d %d %d %d %d %d\n", their_time.sec, their_time.min, their_time.hour, their_time.mday, their_time.month, their_time.year, their_time.wday);
	debug(ausgabe);
	if (their_time.sec == 0) {
		if (their_time.min == 0) {
			if (their_time.hour == 0) {
				if (their_time.mday == 1) {
					if (their_time.month == 1) {
						if (their_time.year == 101) {
							if (their_time.wday == 0) { display_print("All is bon! Welcome to the year 2000!"); display_flip(); return 0;}
						}
					}
				}
			}
		}
	}
	display_print("Something is terribly wrong!\n");
	display_flip();
	return 1;
}
