/**
 *
 * classes.h
 * Common classes definition 
 */

#ifndef __CLASSES__
#define __CLASSES__

enum class_id_nuts {
	GENERIC = 0x00,

	WEATHERSTATION = 0x01,
};

enum class_id_sensors {
	GENERIC 	= 0x00,

	TEMPERATURE = 0x01,
	PRESSURE 	= 0x02,
	HUMIDITY	= 0x03,
};

enum class_id_actors {
	GENERIC 	= 0x80,

	SWITCH		= 0x81,
	DIMMER		= 0x82,
}: 

#endif
