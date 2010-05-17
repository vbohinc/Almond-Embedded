/*
* Core libraries for squirrel
*/
#ifndef SQUIRREL_H
#define SQUIRREL_H
#include "../protocols/downlink/downlink.h"
//#include "../protocols/uplink/uplink.h"


	extern void squirrel_init(void); // Initialize, check for known devices, scan for new devices, start polling/accept pushed messages, output to display, respond to backend requests, log activity
	//void __bluetooth_init();
	
#endif // end of SQUIRREL

