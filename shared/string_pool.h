#ifndef _STRING_POOL_H_
#define _STRING_POOL_H_

#include <avr/io.h>

// EEMEM wird bei aktuellen Versionen der avr-lib in eeprom.h definiert
// hier: definiere falls noch nicht bekannt ("alte" avr-libc)
#ifndef EEMEM
// alle Textstellen EEMEM im Quellcode durch __attribute__ ... ersetzen
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif

extern uint8_t str_error_assert[] EEMEM;
extern uint8_t str_error_info[] EEMEM;
extern uint8_t str_error_warn[] EEMEM;
extern uint8_t str_error_debug[] EEMEM;
extern uint8_t str_error_error[] EEMEM;
extern uint8_t str_uart_frame_error[] EEMEM;
extern uint8_t str_uart_overrun_error[] EEMEM;
extern uint8_t str_uart_buffer_overflow[] EEMEM;
extern uint8_t str_ok[] EEMEM;
extern uint8_t str_error[] EEMEM;
extern uint8_t str_connect[] EEMEM;
extern uint8_t str_disconnect[] EEMEM;
extern uint8_t str_inquiry_end[] EEMEM;
extern uint8_t str_inquiry_results[] EEMEM;
extern uint8_t str_bt_resent_pkg[] EEMEM;
extern uint8_t str_bt_init[] EEMEM;
extern uint8_t str_bt_test_ok[] EEMEM;
extern uint8_t str_bt_test_error[] EEMEM;
extern uint8_t str_bt_timeout_stop[] EEMEM;
extern uint8_t str_bt_req_resent[] EEMEM;
extern uint8_t str_bt_pck_full[] EEMEM;
extern uint8_t str_bt_pck_small[] EEMEM;
extern uint8_t str_bt_crc_error[] EEMEM;


#endif //_STRING_POOL_H_
