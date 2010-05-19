/*
* Core libraries for squirrel
*/
#ifndef SQUIRREL_H
#define SQUIRREL_H
#define SQUIRREL
#include "../protocols/downlink/downlink.h"
#include "../protocols/uplink/uplink.h"

	extern void squirrel_init(void); // Initialize, check for known devices, scan for new devices, start polling/accept pushed messages, output to display, respond to backend requests, log activity
	//void __bluetooth_init();
	extern void squirrel_send_uplink_packet(struct uplink_packet *);
	extern void squirrel_send_downlink_packet(struct downlink_packet *);
	
#endif // end of SQUIRREL

