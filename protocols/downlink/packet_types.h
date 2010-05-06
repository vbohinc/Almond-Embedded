/**
 *
 *
 */


enum downlink_packet_type {
	GET		= 0x10,		// Used by: Squirrel
	SET		= 0x20,     // Used by: Squirrel                                                   
	RET		= 0x30, 	// Used by: Nut
	ECHO	= 0xF0,		// Used for debug, returns the packet
};

enum downlink_packet_type_flags {
	STANDARD = 0x00,	// Standard modifier

	INFO_NUT = 0x01,	// Get the class of the Nut 
	INFO_ID  = 0x02,    // Get the class of the Sensor/Actor
	CONFIG	 = 0x0A,    // Access the configuration
	ADVANCED = 0x0F,	// Advanced features
}
