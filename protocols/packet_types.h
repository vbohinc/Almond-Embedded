/**
 *	Define Packet Types
 *
 */

#ifndef __PACKET_TYPES__
#define __PACKET_TYPES__

enum packet_type {
	GET		 = 0x10,	// Used by: Squirrel/Backend
	SET		 = 0x20,	// Used by: Squirrel/Backend
	RET		 = 0x30, 	// Used by: Nut/Squirrel
	TUNNEL	 = 0xE0,	// To tunnel things through the squirrel
	ECHO	 = 0xF0,	// Used for debug, returns the packet
};

enum packet_type_flags {
	// Nut specific
	STANDARD = 0x00,	// Standard modifier

	INFO_NUT = 0x01,	// Get the class of the Nut 
	INFO_ID  = 0x02,    // Get the class of the Sensor/Actor

	// Squirrel specific
	LOG		 = 0x0A, 	// Returns the latest log data in chronological order
	TIME	 = 0x0B,	// Access the time of the squirrel, (get, set)
	LIST	 = 0x0C, 	// Returns a list of known devices

	// Shared
	CONFIG	 = 0x0F,    // Access the configuration
};

#endif
