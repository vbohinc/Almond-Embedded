#define NUT

#include <bmp085.h>
#include <bluetooth/bluetooth.h>
#include <downlink/downlink.h>
#include <classes.h>

const uint8_t class_id_nut = WEATHERSTATION;
const uint8_t class_id_extensions[] = {TEMPERATURE, PRESSURE};
const uint8_t class_id_extensions_length = 2;


int main (void)
{
        
	while(1)
	{
	}
}
