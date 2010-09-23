#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <avr/io.h>
#include <common.h>

typedef enum {
  BLUETOOTH_MASTER,
  BLUETOOTH_SLAVE
} bt_mode_t;

bool bt_init (void);
bool bt_set_mode (bt_mode_t);
bool bt_receive (uint8_t *data, uint8_t *length);
bool bt_send (uint8_t *data, uint8_t *length);

#ifdef SQUIRREL
bool bt_connect (char *address);
bool bt_disconnect (void);
bool bt_discover (char *result);
#endif /* SQUIRREL */


#endif
