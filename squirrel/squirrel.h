/*
 * squirrel.h
 */

#ifndef __SQUIRREL__
#define __SQUIRREL__

#include "../shared/common.h"
#include "../protocols/uplink/uplink.h"

enum STATES
{
  SLAVE, SLAVE_BUSY, MASTER
};

extern uint8_t squirrel_state_get (void);
extern void    squirrel_state_set (uint8_t state);

extern bool    squirrel_list      (uint8_t num, uplink_payload_list *p);
extern bool    squirrel_log       (uplink_package *p); 


#endif
