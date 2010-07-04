#include "../drivers/storage/sd.h"

uint8_t bytes[3] = {0xAA, 0xAA, 0xAA};

uint8_t main(void) {
	sd_init();
	sd_write_bytes(0, bytes, 3);
	return 0;
}
