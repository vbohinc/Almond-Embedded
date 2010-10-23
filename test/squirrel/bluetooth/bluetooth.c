#include <avr/io.h>
#include <bluetooth/bluetooth.h>


static bool
discover_callback(const char *name, const uint8_t *address)
{
	display_print(name);
	return true;
}

int main(void)
{
	display_init();
	bt_init();
	bt_set_mode(BLUETOOTH_MASTER);
	char[8][6] result;
	display_print("discover:\n");
	bt_discover(result, discover_callback);
}

