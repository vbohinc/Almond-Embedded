#include "string_pool.h"

uint8_t str_error_assert[5] EEMEM 			= "ASS:\0";
uint8_t str_error_info[5] EEMEM		 		= "INF:\0";
uint8_t str_error_warn[5] EEMEM		 		= "WRN:\0";
uint8_t str_error_debug[5] EEMEM	 		= "DBG:\0";
uint8_t str_error_error[5] EEMEM 			= "ERR:\0";
uint8_t str_uart_frame_error[17] EEMEM 		= "UART Frame Error\0";
uint8_t str_uart_overrun_error[19] EEMEM 	= "UART Overrun Error\0";
uint8_t str_uart_buffer_overflow[22] EEMEM 	= "Buffer overflow Error\0";
uint8_t str_ok[3] EEMEM 					= "OK\0";
uint8_t str_error[6] EEMEM 					= "ERROR\0";
uint8_t str_connect[8] EEMEM 				= "CONNECT\0";
uint8_t str_disconnect[11] EEMEM 			= "DISCONNECT\0";
uint8_t str_inquiry_end[12] EEMEM 			= "Inquiry End\0";
uint8_t str_inquiry_results[16] EEMEM 		= "Inquiry Results\0";
uint8_t str_bt_resent_pkg[22] EEMEM 		= "BTM: resent data pkg!\0";
extern uint8_t str_bt_init[17] EEMEM 		= "BTM: Initialized\0";
extern uint8_t str_bt_test_ok[18] EEMEM 	= "BTM: Test Conn=OK\0";
extern uint8_t str_bt_test_error[21] EEMEM 	= "BTM: Test Conn=ERROR\0";


