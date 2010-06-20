#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>
#include "../protocols/downlink/downlink.h"

#define NUT
//#define SQUIRREL

int main (void) {
	struct downlink_packet packet;
	packet.opcode = 0;	
	/*bool test;
	test = downlink_handle_package (&packet);*/
}
