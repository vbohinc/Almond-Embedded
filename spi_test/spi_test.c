#include "../drivers/storage/sd.h"

int[] bytes = {0xAA, 0xAA, 0xAA};

int main(void) {
	sd_init();
	sd_write_bytes(0, bytes, 3);
	return 0;
}
