/**
 * @file squirrel.h
 * the Squirrel header
 */

#ifndef __SQUIRREL__
#define __SQUIRREL__

#include "../shared/common.h"
#include "../protocols/uplink/uplink.h"

///the state for the squirrel
enum STATES
{
    SLAVE, SLAVE_BUSY, MASTER
};

///returns current squirrel state
extern uint8_t squirrel_state_get (void);
///sets current squirrel state
extern void    squirrel_state_set (uint8_t state);

/**
*get a uplink packet with list of devices
*@param num of the number of packet
*@param buffer for the payload packet
*@return true if succeded
*/
extern bool    squirrel_list (uint8_t num, uplink_payload_list *p);

/**
*create an log uplink packet
*@param p bufer for uplink log packet
*@return treu if successful
*/
extern bool    squirrel_log (uplink_package *p);

#define NUTS_LIST 16
#define EXTENSIONS_LIST 16

typedef struct _device_info device_info;

///device info struct, holds mac , class and extensions + values of a device
struct _device_info
{
    char mac[12];

    uint8_t class;
    uint8_t extension_types[EXTENSIONS_LIST];
    uint16_t values_cache[EXTENSIONS_LIST];
};

extern device_info device_list[NUTS_LIST];

#define valid(num) (num < NUTS_LIST && (device_list[num].mac[0] != 0 || device_list[num].mac[1] != 0 || device_list[num].mac[2] != 0 || device_list[num].mac[3] != 0 || device_list[num].mac[4] != 0 || device_list[num].mac[5] != 0 || device_list[num].mac[6] != 0 || device_list[num].mac[7] != 0 || device_list[num].mac[8] != 0 || device_list[num].mac[9] != 0 || device_list[num].mac[10] != 0 || device_list[num].mac[11] != 0))


#endif
