#include <avr/io.h>
#include <bluetooth/bluetooth.h>


static bool
discover_callback(const char *name, const uint8_t *address)
{
	//display address
	return true;
}

int main(void)
{
	bt_init();
	bt_set_mode(BLUETOOTH_MASTER);
	char[8][6] result;
	bt_discover(result, NULL);

}

