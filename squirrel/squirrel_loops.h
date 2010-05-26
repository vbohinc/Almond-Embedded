/*
 * squirrel_loops.h
 *
 *  Created on: May 26, 2010
 *      Author: seanlabastille
 */
#ifndef __SQUIRREL_LOOPS__
#define __SQUIRREL_LOOPS__
#include "squirrel.h"

enum state {
	SLAVE, MASTER
};

void slave_loop();
void master_loop();

#endif
