#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include "downlink.h"


int main (void) {
	struct downlink_packet packet;
	packet.TID = 0;	
	bool test;
	test = downlink_handle_packet (&packet);
	printf("%i", packet.TID);

}
