/*
 * squirrel.c
 *
 *  Created on: May 12, 2010
 *      Author: seanlabastille
 */
#include "squirrel.h"
#include "../drivers/bluetooth/bluetooth.h"
#ifdef SERIAL
#include <stdio.h>
#endif

	// Proposed lookup arrays. Every Nut receives a running number when first encountered
	// I think an unbounded array is a better idea, as we don't need to store these huge bunch of pointers. √
	// These arrays are indexed as follows: [running number]([actor/sensor/configuration ID])
	typedef struct device_info {
		char mac[6];
		uint8_t class;
		uint8_t actor_types[16];
		uint8_t sensor_types[16];
		uint8_t config_types[16];
	} device_info;
	struct device_info* device_list[16]; // FIXME: Unbounded array
#ifndef SERIAL
	int main(void) {
		squirrel_init();
		return 0;
	}
#endif

	void blubb(uint8_t *data_package, const uint8_t callback_type) {
#ifdef SERIAL
		printf("Blubb called: %d", callback_type);
#endif
		}


	void squirrel_init(void) {
		bluetooth_init(blubb);

	}

