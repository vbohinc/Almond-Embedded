/**
 * 
* downlink.h
* Header for libdownlink - The Squirrel-Nuts Communication Protocol
*/

#include <stdint.h>
#include <stdbool.h>

const int DOWNLINK_PACKET_LENGTH = 8;

typedef struct downlink_packet {
	uint8_t 	opcode; 
	uint8_t		payload[DOWNLINK_PACKET_LENGTH -1]; //[DOWNLINK_PACKET_LENGTH - 5];
};

enum downlink_packet_type {
	//INFO	= 0x10,  // Part: Page requested
	//INFORET	= 0x11,  // Page 1: Class ID, 14 Byte ASCII ID, Page 2-32: 7 x [Sensor/Actor/Config ID (1 Byte) | Type (1 Byte) ]
	GET		= 0x20,  // GET 0x21 Class ID | 0x22 0x01-0xFF: Sensor/Actor/Config Type | 0x23 0x01-0xFF: Sensor/Actor/Config Value] 
	SET		= 0x24,                                                        
	SLEEP	= 0x25,
	RET		= 0x26, // RET 0x26 Device CLass ID | 0x27 0x01-0xFF Type | 0x28 0x01-0xFF Value 
	//ERROR	= 0x31
};

// Returns true if valid and handled, false if not valid.




#ifdef SNCP_SQUIRREL
	void downlink_init(int[16] *device_name); // char?
	bool downlink_handle_packet (struct downlink_packet *packet);
#endif

#ifdef SNCP_NUTS
	void downlink_nut_init(int[16] *device_name, &sleep_callback);
//	void downlink_nut_register(int class_number, &callback); // Type is defined through class number
#endif

//
void downlink_handle_package(*downlink_packet);

// void sncp_send_package(sncp_Packet_Type t, &data, &error); Internal use!
*/
