#ifndef _STRING_POOL_H_
#define _STRING_POOL_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

extern prog_char str_error_assert[];
extern prog_char str_error_info[];
extern prog_char str_error_warn[];
extern prog_char str_error_debug[];
extern prog_char str_error_error[];
extern prog_char str_uart_frame_error[];
extern prog_char str_uart_overrun_error[];
extern prog_char str_uart_buffer_overflow[];
extern prog_char str_ok[];
extern prog_char str_error[];
extern prog_char str_connect[];
extern prog_char str_disconnect[];
extern prog_char str_inquiry_end[];
extern prog_char str_inquiry_results[];
extern prog_char str_bt_resent_pkg[];
extern prog_char str_bt_init[];
extern prog_char str_bt_test_ok[];
extern prog_char str_bt_test_error[];
extern prog_char str_bt_timeout_stop[];
extern prog_char str_bt_req_resent[];
extern prog_char str_bt_pck_full[];
extern prog_char str_bt_pck_small[];
extern prog_char str_bt_crc_error[];


#endif //_STRING_POOL_H_
