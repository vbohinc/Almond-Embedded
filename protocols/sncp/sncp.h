/*
* sncp.h
* Header for libsncp - The Squirrel-Nuts Communication Protocol
*/


#define sncp_TYPE // <--- TYPE SWITCH HERE

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

#ifdef SNCP_SQUIRREL
	void sncp_init(uint8[16] *device_name); // char?
#endif

#ifdef SNCP_NUTS
	void sncp_nut_init(uint8[16] *device_name, &sleep_callback);
	void sncp_nut_register(uint8 class, &callback); // Type is defined through class number
#endif

const uint8 sncp_packet_length = 32;
void sncp_handle_package();

// void sncp_send_package(sncp_Packet_Type t, &data, &error); Internal use!

