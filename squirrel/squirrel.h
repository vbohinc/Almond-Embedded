/*
* Core libraries for squirrel
*/

#ifndef __SQUIRREL_H
#define __SQUIRREL_H

#include <assert.h>
#include "../drivers/bluetooth/bluetooth.h"
//#include "../drivers/storage/storage.h"
//#include "../drivers/display/display.h"
//#include "../drivers/platform/platform.h"
#include "../protocols/downlink/downlink.h"
#include "../protocols/uplink/uplink.h"
#include "../protocols/package_types.h"

typedef struct _device_info device_info;
struct _device_info {
		uint8_t mac[6];
		uint8_t class;
		uint8_t actuator_types[16];
		uint8_t sensor_types[16];
		uint8_t config_types[16];
};

struct device_info *device_list[16];

enum states {
	SLAVE, MASTER
};

uint8_t state;

#endif
