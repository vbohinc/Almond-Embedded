/*
* Core libraries for squirrel
*/
#ifndef SQUIRREL_H
#define SQUIRREL_H
#include "../protocols/downlink/downlink.h"
//#include "../protocols/uplink/uplink.h"


#define SQUIRREL // We're on the Squirrel here, so we'll be operating DOWNLINK in Master mode


	extern void squirrel_init(); // Initialize, check for known devices, scan for new devices, start polling/accept pushed messages, output to display, respond to backend requests, log activity
	//void __bluetooth_init();
	
#endif // end of SQUIRREL

