/*
* downlink.h
* Header for libdownlink - The Squirrel-Nuts Communication Protocol
*/

#include <stdint.h>
#include <stdbool.h>

#define DOWNLINK_PACKET_LENGTH 32

typedef struct downlink_packet {
	uint16_t 	TID; 
	uint8_t 	opcode; 
	uint8_t		payload; //[DOWNLINK_PACKET_LENGTH - 5];
	uint16_t	checksum;
};

// #define sncp_TYPE // <--- TYPE SWITCH HERE

enum sncp_Packet_Type {
	INFO	= 0x10, 
	INFORET	= 0x11, 
	GET		= 0x20,
	SET		= 0x21, 
	SLEEP	= 0x23,
	RET		= 0x22, 
	ACK		= 0x30, 
	ERROR	= 0x31
};

// Returns true if valid and handled, false if not valid.

bool downlink_handle_packet (struct downlink_packet *packet);

/*
#ifdef SNCP_SQUIRREL
//	void downlink_init(int[16] *device_name); // char?
#endif

#ifdef SNCP_NUTS
//	void downlink_nut_init(int[16] *device_name, &sleep_callback);
//	void downlink_nut_register(int class_number, &callback); // Type is defined through class number
#endif

//
void downlink_handle_package(*downlink_packet);

// void sncp_send_package(sncp_Packet_Type t, &data, &error); Internal use!
*/
