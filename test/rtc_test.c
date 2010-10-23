#include "../drivers/rtc/rtc.h"

int main(int argc, char *argv) {
	time_t my_time = {59,59,23,31,12,99,6};
	set_time (&my_time);
	sleep_ms(1000);
	time_t their_time = get_time();
	if (their_time.sec == 0) {
		if (their_time.min == 0) {
			if (their_time.hour == 0) {
				if (their_time.mday == 1) {
					if (their_time.month == 1) {
						if (their_time.year == 100) {
							if (their_time.wday == 0) { display_print("All is bon! Welcome to the year 2000!"); return 0;}
						}
					}
				}
			}
		}
	}
	display_print("Something is terribly wrong!");
	return 1;
}