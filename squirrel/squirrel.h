/*
 * squirrel.h
 */

#ifndef __SQUIRREL__
#define __SQUIRREL__

#include "../shared/common.h"

enum STATES
{
  SLAVE, SLAVE_BUSY, MASTER
};

extern uint8_t squirrel_state_get (void);
extern void    squirrel_state_set (uint8_t state);

#endif
