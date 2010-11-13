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
extern bool bt_receive (void * data, uint8_t *length, uint16_t timeout_ms);
extern bool bt_send (void * data, const uint8_t length);

#ifdef SQUIRREL

extern bool bt_connect (const char *address);
extern bool bt_disconnect (void);
extern bool bt_discover (char result[8][12], void (*update_callback)(const uint8_t progress));
#endif /* SQUIRREL */


#endif
