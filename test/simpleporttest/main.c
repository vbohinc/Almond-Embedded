#include <avr/io.h>
#include <shared/common.h>

int main (void)
{
	PORTE.DIR = 0xFF;
	PORTE.OUT = 0x02;
	while(true);
}
