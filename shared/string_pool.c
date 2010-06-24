#include "string_pool.h"

prog_char str_error_assert[5]  			= "ASS:\0";
prog_char str_error_info[5] 		 		= "INF:\0";
prog_char str_error_warn[5] 		 		= "WRN:\0";
prog_char str_error_debug[5] 	 		= "DBG:\0";
prog_char str_error_error[5]  			= "ERR:\0";
prog_char str_uart_frame_error[17]  		= "UART Frame Error\0";
prog_char str_uart_overrun_error[19]  	= "UART Overrun Error\0";
prog_char str_uart_buffer_overflow[22]  	= "Buffer overflow Error\0";
prog_char str_ok[3]  					= "OK\0";
prog_char str_error[6]  					= "ERROR\0";
prog_char str_connect[8]  				= "CONNECT\0";
prog_char str_disconnect[11]  			= "DISCONNECT\0";
prog_char str_inquiry_end[12]  			= "Inquiry End\0";
prog_char str_inquiry_results[16]  		= "Inquiry Results\0";
prog_char str_bt_resent_pkg[22]  		= "BTM: resent data pkg!\0";
prog_char str_bt_init[17]  				= "BTM: Initialized\0";
prog_char str_bt_test_ok[18]  			= "BTM: Test Conn=OK\0";
prog_char str_bt_test_error[21]  		= "BTM: Test Conn=ERROR\0";
prog_char str_bt_timeout_stop[30]  		= "BTM: Timeout (STOP). Reorder.\0";
prog_char str_bt_req_resent[21]  		= "BTM: Resent request.\0";
prog_char str_bt_pck_full[28]  			= "BTM: Package full. Reorder.\0";
prog_char str_bt_pck_small[29]  			= "BTM: Pkg too small. Reorder.\0";
prog_char str_bt_crc_error[25]  			= "BTM: CRC error. Reorder.\0";


