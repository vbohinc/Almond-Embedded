#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <avr/io.h>
#include <common.h>

typedef enum {
  BLUETOOTH_MASTER,
  BLUETOOTH_SLAVE
} bt_mode_t;

extern bool bt_init (void);
extern bool bt_set_mode (const bt_mode_t);
extern bool bt_receive (uint8_t* data, uint8_t* length);
extern bool bt_send (const char* data, const uint8_t length);

#ifdef SQUIRREL
extern bool bt_connect (const char *address);
extern bool bt_disconnect (void);
extern bool bt_discover (const char *result);
#endif /* SQUIRREL */


#endif
