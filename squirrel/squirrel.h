/**
 * squirrel.h - the Squirrel
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
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

extern bool    squirrel_list (uint8_t num, uplink_payload_list *p);
extern bool    squirrel_log (uplink_package *p);


#define NUTS_LIST 16
#define EXTENSIONS_LIST 16

struct _device_info
{
    char mac[12];

    uint8_t class;
    uint8_t extension_types[EXTENSIONS_LIST];
    uint16_t values_cache[EXTENSIONS_LIST];
};

typedef struct _device_info device_info;
extern device_info device_list[NUTS_LIST];

#define valid(num) (num < NUTS_LIST && (device_list[num].mac[0] != 0 || device_list[num].mac[1] != 0 || device_list[num].mac[2] != 0 || device_list[num].mac[3] != 0 || device_list[num].mac[4] != 0 || device_list[num].mac[5] != 0 || device_list[num].mac[6] != 0 || device_list[num].mac[7] != 0 || device_list[num].mac[8] != 0 || device_list[num].mac[9] != 0 || device_list[num].mac[10] != 0 || device_list[num].mac[11] != 0))


#endif
