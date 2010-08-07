/**
 *  @addtogroup BluetoothBTM222 Bluetooth Library
 *  @code #include <bluetooth.h> @endcode
 *
 *  @brief This file contanis the definition for the functions available to communicate with the Bluetooth Module over USART.
 *
 *  Some of the functions are only available on the Squirrel (define: SQUIRREL) because on the nut isn't enough program memory available.
 *
 *
 *
 *  @note Used Bluetooth Module: BTM-222
 *  @author Stefan Profanter
 *  @date 26.04.2010
 *
 *
 *  @see uart.c
 */

/**@{*/

#include "bluetooth.h"
#include <string.h>
#include <stdlib.h>
#include "ftdi.h"
#include <util/delay.h>

#include <avr/pgmspace.h>

/**
 * FIFO to store received data. Uses bluetooth_inbuffer to store bytes
 */
static fifo_t bluetooth_infifo;

/**
 * Buffer for the FIFO to put received data into.
 */
static uint8_t bluetooth_inbuffer[BLUETOOTH_RECEIVE_BUFFER_SIZE];

uint8_t bluetooth_is_connected = 0;

/**
 * Array to put data package to send into.
 * Also Callback is called with this array to return a received package.
 * This array is also used to return a response of a command.
 * The biggest response is from the ATF? command.
 * The maximum number of found devices is: 8
 * The maximum lenght of a name is: 16
 * The length of an address is: 6 (compressed format, without '-', each byte contains 2 chars)
 * The data is stored in the following format (where x is the index of found device: Device '1' is at index '0'):
 * arr[0]: Number of found devices
 * arr[1+x*(16+6)] to arr[1+x*(16+6)+15]: Name of found device.
 * 		Ex: arr[1] to arr[15] contains first name terminated by null if shorter than 16 chars
 * arr[1+x*(16+6)+16] to arr[1+x*(16+6)+21]: address of found device.
 * In total there are 1+(16+6)*8=177 bytes needed.
 *
 * The ATF? command is only available on Squirrel to save memory.
 * On Nut the maximum size is approx. the DISCONNECT response
 */
#ifdef SQUIRREL
char bluetooth_data_package[177];
#else
char bluetooth_data_package[35];
#endif

/**
 * Index in the bluetooth_data_package array to store new received byte (if byte is part of a datapackage)
 */
static uint8_t bluetooth_data_package_index=0;

/**
 * Previous processed byte or -1 if no byte jet received.
 * Used to detect stop byte and masked special bytes
 */
static int16_t bluetooth_previous_byte=-1;


/**
 * Size of Buffer to build cmd to send.
 * Should be at least 36 bytes (maximum length of one line of response ATF? )
 */
#define BLUETOOTH_CMD_BUFFER_SIZE 36

/**
 * Buffer to build cmd to send or to parse resonse of bluetooth device.
 */
static char bluetooth_cmd_buffer[BLUETOOTH_CMD_BUFFER_SIZE];

/**
 * Contains 3rd and 4th byte of the send command. Ex. for ATF? contains 'F?'.
 * Used to check if received data in online mode is a response of the module to a sent command.
 */
static char bluetooth_cmd_sent[2];

/**
 * Current index to write received byte (if byte is part of command response)
 */
static uint8_t bluetooth_cmd_buffer_head = 0;

/**
 * When a response for a command comes in, this variable will be set to the value of the response.
 * @see bluetooth_cmd_wait_response
 */
static uint8_t bluetooth_response_code = 0;

/**
 * Contains the length of the recently sent package to resent it if needed. The data is stored in bluetooth_data_package
 */
static uint8_t bluetooth_sent_length = 0;

/**
 * Array to copy response package into. If NULL, no response array expected
 */
static uint8_t *bluetooth_wait_response_array = NULL;

/**
 * Pointer to copy response package length into. If NULL, no response array expected
 */
static uint8_t *bluetooth_wait_response_length = NULL;

/**
 * timeout to wait for stop byte. In ms.
 * If timeout occurs the resent byte will be sent to the connected device.
 */
#define BLUETOOTH_STOP_BYTE_TIMEOUT 1000

/**
 * when new data package begin received, bluetooth_ms_to_timeout will be set to BLUETOOTH_STOP_BYTE_TIMEOUT.
 * If no bytes received for BLUETOOTH_STOP_BYTE_TIMEOUT ms, a timeout occurs and package will be re-requested because there was no stop byte or there was a transmission error.
 */
static int32_t bluetooth_ms_to_timeout = -1; //disable timeout


/**
 * Used for the bluetooth_send_datapackage to wait until the remote end responded with a package or timeout occurs.
 */
static uint8_t bluetooth_package_received = 0;

//#define UART_BAUD_RATE      9600
#define UART_BAUD_RATE      19200


void bluetooth_init(void (*bluetooth_callback_handler)(char *data_package, const uint8_t callback_type, const uint8_t data_length))
{
	bluetooth_callback = bluetooth_callback_handler;
	fifo_init (&bluetooth_infifo,   bluetooth_inbuffer, BLUETOOTH_RECEIVE_BUFFER_SIZE);
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
}

void bluetooth_input_to_array(void)
{
	//Read bytes from UART input buffer
	unsigned int c;

	do
	{
		c =  uart_getc();
		if ( c & UART_FRAME_ERROR )
		{
			/* Framing Error detected, i.e no stop bit detected */
			warn_pgm(PSTR("FRM ERR")); //Frame Error
		}
		if ( c & UART_OVERRUN_ERROR )
		{
			/*
			 * Overrun, a character already present in the UART UDR register was
			 * not read by the interrupt handler before the next character arrived,
			 * one or more received characters have been dropped
			 */
			warn_pgm(PSTR("OVR ERR")); //Overrun Error
		}
		if ( c & UART_BUFFER_OVERFLOW )
		{
			/*
			 * We are not reading the receive buffer fast enough,
			 * one or more received character have been dropped
			 */
			warn_pgm(PSTR("BUF ERR")); //Buffer Overflow error
		}
		else if (c!=UART_NO_DATA)
		{
			_inline_fifo_put (&bluetooth_infifo, c);
		}

	} while (c!=UART_NO_DATA && c!= UART_FRAME_ERROR && c!= UART_OVERRUN_ERROR && c!=UART_BUFFER_OVERFLOW);
}

/**
 * Processes received response from the BTM-222 Module.
 */
static void bluetooth_process_response(void)
{
	//received response is empty
	if (bluetooth_cmd_buffer_head == 0)
		return;
	if (strncmp_P(bluetooth_cmd_buffer, PSTR("OK"), 2)==0)
	{
		//Special handling for ATF, because OK is only, when Inquiry-End message received
		if (bluetooth_cmd_sent[0]!='F')
			bluetooth_response_code = 1;
		return;
	}
	bluetooth_data_package[0]=0; //Empty return array
	if (strncmp_P(bluetooth_cmd_buffer, PSTR("ERROR"), 5)==0)
	{
		bluetooth_response_code = 4;
		return;
	}
	if (strncmp_P(bluetooth_cmd_buffer, PSTR("CONNECT"), 7)==0)
	{

		//remove remaining bytes
		_delay_ms(100);
		bluetooth_input_to_array();
		while (bluetooth_infifo.count>0)
		{
			fifo_get_nowait(&bluetooth_infifo);
		}

		bluetooth_address_to_array(bluetooth_cmd_buffer+10, bluetooth_data_package, 1);
		bluetooth_is_connected = 1;
		bluetooth_response_code = 2;
		//Call callback function
		bluetooth_callback(bluetooth_data_package, 1, 6);
		return;
	}
	if (strncmp_P(bluetooth_cmd_buffer, PSTR("DISCONNECT"), 10)==0)
	{
		//remove remaining bytes
		_delay_ms(100);
		bluetooth_input_to_array();
		while (bluetooth_infifo.count>0)
		{
			fifo_get_nowait(&bluetooth_infifo);
		}

		bluetooth_address_to_array(bluetooth_cmd_buffer+13,bluetooth_data_package,1);
		bluetooth_is_connected =  0;
		bluetooth_response_code = 3;
		//Call callback function
		bluetooth_callback(bluetooth_data_package, 2, 6);
		return;
	}

	switch (bluetooth_cmd_sent[0])
	{
	case 'A':
		strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received timeout error into bluetooth_data_package
		bluetooth_response_code = 4;
		break;
	case 'B':
		//copy received address into return array without '-'
		bluetooth_address_to_array(bluetooth_cmd_buffer,bluetooth_data_package , 1);
		break;
	case 'C':
		strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'D':
		//copy received address into return array without '-'
		bluetooth_address_to_array(bluetooth_cmd_buffer,bluetooth_data_package,  1);
		break;
	case 'E':
		strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'F':
		if (strncmp_P(bluetooth_cmd_buffer, PSTR("Inquiry End"), 11)==0)
		{
			bluetooth_data_package[0] =  bluetooth_cmd_buffer[13]-48; //Get char of count found devices and convert to number
			bluetooth_response_code = 1;
			return;
		}
		if (strncmp_P(bluetooth_cmd_buffer, PSTR("Inquiry Results"), 15)==0)
		{
			return;
		}
		//Received format:
		//1  NAME1         2034-04-16DBF1
		uint8_t index = bluetooth_cmd_buffer[0]-48; //Get first char and convert it to a number
		index--; //Index of NAME1 is 0 not 1

		//Copy name
		for (uint8_t i=0; i<16; i++)
			bluetooth_data_package[1+index*(16+6)+i] = bluetooth_cmd_buffer[i+3];

		//Copy address
		bluetooth_address_to_array(bluetooth_cmd_buffer+(1+index*(16+6)+16), bluetooth_data_package+21, 1);

		break;
	case 'H':
		if (bluetooth_cmd_sent[1]==0)
		{ //Connection dropped. Response already handled: OK and DISCONNECT
			return;
		}
		else
			strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'N':
		strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received name into return array
		break;
	case 'O':
		if (bluetooth_cmd_sent[1]==0)
		{ //Connection reopened. Response already handled: OK
			return;
		}
		else
			strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'P':
		strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received pin into return array
		break;
	case 'R':
		strcpy(bluetooth_data_package,bluetooth_cmd_buffer); //copy received value into return array
		break;
	case '+':
		break;
	default:
		break;
	}
}

int inline bluetooth_putc(const uint8_t byte)
{
	return uart_putc(byte);
}

/**
 * Resents the byte from bluetooth_data_package because the remote end asked to do so (caused by a timeout)
 */
static void bluetooth_resent_package(void)
{
	for (uint8_t i=0; i<bluetooth_sent_length; i++)
	{
		if (bluetooth_putc(bluetooth_data_package[i])==0)
			break;
	}
}


/**
 * Own implementation of strstr function to check if bluetooth_data_package contains DISCONNECT.
 */
static uint8_t bluetooth_is_disconnect_msg(void)
{
	uint8_t d = 'D';
	prog_char *str = PSTR("ISCONNECT");
	for (uint8_t i=0; i<bluetooth_data_package_index-9; i++)
	{
		if (bluetooth_data_package[i] == d)
		{
			uint8_t j = 0;
			for (j=0; j<9; j++)
			{
				uint8_t myByte = pgm_read_byte(str+j);
				if (bluetooth_data_package[i+j+1] != myByte)
					break;
			}
			if (j==9)
			{
				return 1;
			}
		}
	}
	return 0;

}

void bluetooth_process_data(void)
{
	bluetooth_input_to_array();


	while (bluetooth_infifo.count>0)
	{ //read all bytes from fifo
		//Check if received data is a datapackage or a response to a sent command.
		//ATH, ATO, and ATI2 are also available when connection is open.
		if ((bluetooth_is_connected==0) ||
		  (bluetooth_cmd_sent[0]=='+' && bluetooth_cmd_sent[1]=='+') ||
		  (bluetooth_cmd_sent[0]=='H' && bluetooth_cmd_sent[1]==13) ||
		  (bluetooth_cmd_sent[0]=='O' && bluetooth_cmd_sent[1]==13) ||
		  (bluetooth_cmd_sent[0]=='I' && bluetooth_cmd_sent[1]=='2'))
		{
			//Data in FIFO is response to a sent command
			int16_t byte = fifo_get_nowait(&bluetooth_infifo);
			//error_putc(byte);

			if (byte == 13 || byte == 10)
			{
				//handle received command
				bluetooth_cmd_buffer[bluetooth_cmd_buffer_head]=0;
				bluetooth_process_response();
				bluetooth_cmd_buffer_head = 0;
			} else if (byte>0 && byte <=255)
			{
				bluetooth_cmd_buffer[bluetooth_cmd_buffer_head] = byte;
				bluetooth_cmd_buffer_head++;
				if (bluetooth_cmd_buffer_head == BLUETOOTH_CMD_BUFFER_SIZE)
				{
					bluetooth_cmd_buffer_head = 0;
					bluetooth_response_code = 5; //No valid response
				}
			}
			bluetooth_input_to_array();

		} else {

			//will be broken when package is complete or full
			while (1)
			{
				bluetooth_input_to_array();
				//Data in FIFO is a data package for the callback function
				int16_t byte = fifo_get_nowait(&bluetooth_infifo);

				//error_putc(byte);

				if (byte == -1)
				{
					if (bluetooth_ms_to_timeout>=1)
					{
						//it is waiting for stop byte. Check timeout
						bluetooth_ms_to_timeout--; //count down timeout
						if (bluetooth_ms_to_timeout == 0)
						{
							bluetooth_ms_to_timeout = -1; //disable timeout

							bluetooth_data_package_index=0;

							//reorder package
							bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
							bluetooth_putc(BLUETOOTH_RESENT_BYTE);
						}

						_delay_ms(1);
					} else {
						return;
					}
				}
				else
				{
					//check if module sent disconnect message
					if (bluetooth_data_package_index >= 10 && bluetooth_is_disconnect_msg()==1)
					{
						//data was DISCONNECT message from module
						 if(byte == 10 || byte == 13)
						{ //full disconnect message received
							uint8_t i;
							uint8_t offset = 0;
							for (offset=0; offset<bluetooth_data_package_index-28;offset++)
							{
								if (bluetooth_cmd_buffer[offset]=='D')
									break;
							}
							for (i =0 ; i<28; i++)
							{
								bluetooth_cmd_buffer[i] = bluetooth_data_package[i+offset];
							}
							bluetooth_cmd_buffer_head = i;
							bluetooth_cmd_buffer[bluetooth_cmd_buffer_head]=0;
							bluetooth_process_response();
							bluetooth_cmd_buffer_head = 0;
							bluetooth_data_package_index=0;
							break;
						} else {
							bluetooth_data_package[bluetooth_data_package_index] = (uint8_t)byte;
							bluetooth_previous_byte = byte;
							bluetooth_data_package_index++;
						}
					}
					else
					{
						if (byte == BLUETOOTH_SPECIAL_BYTE && bluetooth_previous_byte!=BLUETOOTH_SPECIAL_BYTE)
						{
							//bluetooth was special byte. maybe it was a special command. check next byte.
							bluetooth_previous_byte = BLUETOOTH_SPECIAL_BYTE;
							bluetooth_ms_to_timeout = BLUETOOTH_STOP_BYTE_TIMEOUT; //reset timeout
						}
						else if (byte == BLUETOOTH_STOP_BYTE  && bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE)
						{
							//Handle array because last byte was stop byte

							bluetooth_ms_to_timeout = -1; //disable timeout
							bluetooth_previous_byte = -1;

							bluetooth_handle_array();
							bluetooth_data_package_index=0;
							break;
						}
						else if (byte == BLUETOOTH_RESENT_BYTE  && bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE)
						{
							bluetooth_ms_to_timeout = -1; //disable timeout
							bluetooth_previous_byte = -1;

							bluetooth_data_package_index=0;
							bluetooth_resent_package();
							break;
						}
						else if (bluetooth_data_package_index < BLUETOOTH_DATA_PACKAGE_SIZE-1)
						{

							bluetooth_ms_to_timeout = BLUETOOTH_STOP_BYTE_TIMEOUT; //reset timeout
							bluetooth_previous_byte = byte;

							if ((byte == BLUETOOTH_SPECIAL_BYTE) && (bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE))
							{ //Double special byte means that there is one byte in the data package with the value of special byte
								bluetooth_previous_byte = -1; //to make sure next time this if can't be called and the previous STOP
								//and RESENT if canno't be called
							}

							bluetooth_data_package[bluetooth_data_package_index] = (uint8_t)byte;
							bluetooth_data_package_index++;

							if (bluetooth_data_package_index >= BLUETOOTH_DATA_PACKAGE_SIZE)
							{

								bluetooth_ms_to_timeout = -1; //disable timeout
								//array is full, handle it
								bluetooth_handle_array();
								bluetooth_data_package_index=0;
								break;
							}
						} else {
							//BTM: Package full. Reorder
							//flush fifo
							while (bluetooth_infifo.count>0)
								fifo_get_nowait(&bluetooth_infifo);
							bluetooth_ms_to_timeout = -1; //disable timeout
							bluetooth_data_package_index=0;

							//reorder package

							bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
							bluetooth_putc(BLUETOOTH_RESENT_BYTE);

							break;
						}
					}
				}
			}
			bluetooth_previous_byte = -1;
		}
	}
}

void bluetooth_handle_array(void)
{

	if (bluetooth_wait_response_array != NULL)
	{
		(*bluetooth_wait_response_length) = bluetooth_data_package_index;
		//copy received package to return array
		for (uint8_t i=0; i<bluetooth_data_package_index; i++)
			bluetooth_wait_response_array[i] = bluetooth_data_package[i];

		bluetooth_package_received = 1;
	}
	else
	{
		//Call callback function
		bluetooth_callback(bluetooth_data_package, 0, bluetooth_data_package_index);
	}
}

uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length)
{
	debug_pgm(PSTR("P SND:"));
	for (uint8_t i=0; i<length; i++)
	{
		byte_to_hex(data[i]);
		error_putc(' ');
	}
	error_putc(13);
	bluetooth_wait_response_array = NULL;
	bluetooth_wait_response_length = NULL;

	bluetooth_cmd_buffer[0]=0;
	uint8_t error = 0;

	//send data and store send bytes to bluetooth_data_package to resent full package if requested
	bluetooth_data_package_index = 0;

	for (uint8_t i=0; i<length && error==0; i++)
	{
		if (data[i]==BLUETOOTH_SPECIAL_BYTE)
		{
			error = (error || (bluetooth_putc(BLUETOOTH_SPECIAL_BYTE)==0));
			bluetooth_data_package[bluetooth_data_package_index] = BLUETOOTH_SPECIAL_BYTE;
			bluetooth_data_package_index++;
		}
		error = (error || (bluetooth_putc(data[i])==0));
		bluetooth_data_package[bluetooth_data_package_index] = data[i];
		bluetooth_data_package_index++;
	}

	bluetooth_package_received = 0;


	bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
	bluetooth_putc(BLUETOOTH_STOP_BYTE);

	bluetooth_data_package[bluetooth_data_package_index] = BLUETOOTH_SPECIAL_BYTE;
	bluetooth_data_package_index++;
	bluetooth_data_package[bluetooth_data_package_index] = BLUETOOTH_STOP_BYTE;
	bluetooth_data_package_index++;
	bluetooth_sent_length = bluetooth_data_package_index;
	bluetooth_data_package_index = 0;

	if (error)
			return 1;
	else
		return 0;
}

uint8_t bluetooth_send_data_package_with_response(uint8_t *data, uint8_t *length, const uint16_t timeout_ms)
{

	if (bluetooth_send_data_package(data, *length)==1)
		return 1;


	bluetooth_wait_response_array = data;
	bluetooth_wait_response_length = length;




	uint16_t ms_to_timeout = timeout_ms;


	while (!bluetooth_package_received && ms_to_timeout>0)
	{
		//wait a millisecond
		_delay_ms(1);
		ms_to_timeout--;
		if (ms_to_timeout == 0)
		{
			//timeout!!
			return 2;
		}
		//check if package is available
		bluetooth_process_data();
	}


	//data already copied in handle_array fkt
	bluetooth_wait_response_array = NULL;
	bluetooth_wait_response_length = NULL;
	return 0;


	return 0;
}

uint8_t bluetooth_set_as_master(void)
{
	char* currentMode = bluetooth_cmd_get_mode();
	if (currentMode == NULL)
		return 0;
	if (currentMode[0] == '0') //already in master mode
		return 1;
	if (bluetooth_cmd_set_mode(0)==0)
		return 0;
	if (bluetooth_cmd_autoconnect(0)==0)
		return 0;
	else
		return 1;

}

uint8_t bluetooth_set_as_slave(void)
{
	char* currentMode = bluetooth_cmd_get_mode();
	if (currentMode == NULL)
		return 0;
	if (currentMode[0] == '1') //already in slave mode
		return 1;
	if (bluetooth_cmd_set_remote_address(NULL)==0)
		return 0;

	if (bluetooth_cmd_autoconnect(1)==0)
		return 0;
	if (bluetooth_cmd_set_mode(1)==0)
		return 0;
	else
		return 1;

}

uint8_t bluetooth_test_connection(uint8_t tries)
{

	uint8_t i;
	for (i=0; i<tries; i++)
	{
		if (bluetooth_cmd_test_connection()==1)
			break;
	}
	return (i<tries);
}


/**
 * Converts a char value 0-9,A-F,a-f to its corresponding int value
 * @param chr the char value
 * @return the int value
 */
static uint8_t char_to_hex(uint8_t chr)
{
	//convert char number to int
	if (chr>='0' && chr <='9')
		return chr-'0';
	else if (chr>='A' && chr<='F')
		return chr-'A'+10;
	else if (chr>='a' && chr<='f')
		return chr-'a'+10;
	else
		return 255;
}

/**
 * Converts a hex value to its corresponding char value 0-9,A-F
 * @param hex the hex value
 * @return the char value
 */
static uint8_t hex_to_char(uint8_t hex)
{
	if (hex<=9)
		return hex+'0';
	else if (hex>9 && hex <= 15)
		return hex+'A'-10;
	else
		return 0;
}


void bluetooth_address_to_array(const char *full_address, char *compressed_address, const uint8_t address_with_hyphen)
{
	uint8_t addr_buffer = 0;
	uint8_t buf_idx = 0;

	for (uint8_t i=0; i<6; i++)
	{
		addr_buffer |= ((char_to_hex(full_address[buf_idx])<<4)& 0xF0);
		buf_idx++;
		addr_buffer |= (char_to_hex(full_address[buf_idx])& 0xF);
		buf_idx++;
		compressed_address[i] = addr_buffer;
		addr_buffer = 0;
		if (address_with_hyphen==1 && (i==1 || i==2))
		{
			buf_idx++;//leave '-'
		}
	}
}

void bluetooth_array_to_address(const char *compressed_address, char *full_address, const uint8_t address_with_hyphen)
{

	uint8_t addr_buffer = 0;
	uint8_t buf_idx = 0;

	for (uint8_t i=0; i<6; i++)
	{
		addr_buffer = compressed_address[i]; //contains 2 hex values
		full_address[buf_idx+1] = hex_to_char(addr_buffer & 0xF);
		addr_buffer = (addr_buffer >> 4);
		full_address[buf_idx] = hex_to_char(addr_buffer & 0xF);

		buf_idx+=2;
		if (address_with_hyphen==1 && (i==1 || i==2))
		{
			full_address[buf_idx] = '-';
			buf_idx++;//'-' inserted
		}
	}
}


//---------------------------------------------------------
//
//               Bluetooth Commands
//
//---------------------------------------------------------

/**
 * Sends the command to the bluetooth module with the specified delay_ms between each byte.
 * @param cmd The command as char array with nullbyte. Ex: 'ATL1'
 * @param delay_ms milliseconds to wait between each byte.
 * @return 1 on success, 0 on failure (timeout)
 */
static uint8_t bluetooth_cmd_send_delay (const char* cmd, const uint16_t delay_ms)
{
	bluetooth_response_code = 0;



	bluetooth_cmd_sent[0] = cmd[2];
	bluetooth_cmd_sent[1] = cmd[3];

	for (uint8_t i =0; cmd[i]!='\0'; i++)
	{

		if (bluetooth_putc(cmd[i]) == 0)
		{
			return 0;
		}

		_delay_ms(delay_ms);
	}

	return 1;
}

/**
 * Sends the command to the bluetooth module.
 * @param cmd The command as char array with nullbyte. Ex: 'ATL1'
 * @return 1 on success, 0 on failure (timeout)
 */
static uint8_t bluetooth_cmd_send (const char* cmd)
{
	//The delay of 42ms ist a result of a test run, where the delay time is now the minimum possible
	return bluetooth_cmd_send_delay(cmd,42);
}

/**
 * Waits until the bluetooth device returns one of the following responses. For each response will be returned a number which is given in the brackets:
 * @li OK (1)
 * @li CONNECT (2)
 * @li DISCONNECT (3)
 * @li ERROR (4)
 * @return The number of the response.
 */
static uint8_t bluetooth_cmd_wait_response (void)
{
	while (bluetooth_response_code == 0) //Wait until bluetooth devices has responsed (handled by interrupt)
	{

		bluetooth_process_data();
	}
	return bluetooth_response_code;
}

uint8_t bluetooth_cmd_test_connection (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("AT\r"));
	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;
	return (bluetooth_cmd_wait_response()==1); //OK

}

#ifdef SQUIRREL
uint8_t bluetooth_cmd_connect (const uint8_t dev_num)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATA\r"));
	if (dev_num>0)
	{
		bluetooth_cmd_buffer[3] = dev_num+48; //Convert number to number as char
		bluetooth_cmd_buffer[4] = 13; //<CR>
		bluetooth_cmd_buffer[5] = 0;
        }
	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1)//OK
		return 0;

	bluetooth_response_code = 0;

	return (bluetooth_cmd_wait_response()==2); //CONNECT
}

char* bluetooth_cmd_get_address (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATB?\r"));

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}
#endif

uint8_t bluetooth_cmd_set_remote_address (const char* address)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATD0\r"));
	if(address != NULL)
        {
		bluetooth_cmd_buffer[3]='=';

		bluetooth_array_to_address(address,bluetooth_cmd_buffer+4,0);

		bluetooth_cmd_buffer[16] = 13; //<CR>
		bluetooth_cmd_buffer[17] = 0;

	}

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()==1) //OK
	{
		_delay_ms(2000);
		return 1;
	} else
		return 0;
}

#ifdef SQUIRREL

char* bluetooth_cmd_search_devices (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATF?\r"));


	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}

uint8_t bluetooth_cmd_close_connection (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATH\r"));
	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK
}

uint8_t bluetooth_cmd_discoverable (const uint8_t discoverable)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATH?\r")); //? will be replaced 
	bluetooth_cmd_buffer[3] = discoverable+48; //Convert number to number as char
	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK
}

uint8_t bluetooth_cmd_set_name (const char *name)
{
	if (name == NULL)
		return 0;

        strcpy_P(bluetooth_cmd_buffer,PSTR("ATN=\r"));
	strcpy(bluetooth_cmd_buffer+4,name);
        strcat_P(bluetooth_cmd_buffer,PSTR("\r"));

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
			return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

char* bluetooth_cmd_get_name (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATN?\r"));

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;

}

#endif

uint8_t bluetooth_cmd_autoconnect (const uint8_t autoconnect)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATO1\r"));
	if (autoconnect == 1)
		bluetooth_cmd_buffer[3] = '0';
	
        if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()==1) //OK
	{
		_delay_ms(3000);
		return 1;
	} else
		return 0;
}

#ifdef SQUIRREL

uint8_t bluetooth_cmd_set_pin (const char *pin)
{
	if (pin == NULL)
		return 0;
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATP\r"));
        if(pin[0] != 0)
        {
            bluetooth_cmd_buffer[3] = '=';
            strcpy(bluetooth_cmd_buffer+4,pin);
            strcat_P(bluetooth_cmd_buffer,PSTR("\r"));
        }

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
			return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

char* bluetooth_cmd_get_pin (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATP?\r"));

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;

}

#endif

uint8_t bluetooth_cmd_set_mode (uint8_t mode)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATR?\r")); // ? will be replaced
	bluetooth_cmd_buffer[3] = mode+48; //Convert number to number as char
	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()==1) //OK
	{
		_delay_ms(3500);
		return 1;
	} else
		return 0;

}

char* bluetooth_cmd_get_mode (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATR?\r"));

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return NULL;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}


#ifdef SQUIRREL
uint8_t bluetooth_cmd_restore_settings (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("ATZ0\r"));
	return (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0);

}

uint8_t bluetooth_cmd_online_command (void)
{
        strcpy_P(bluetooth_cmd_buffer,PSTR("++++"));

	if (bluetooth_cmd_send_delay(bluetooth_cmd_buffer, 1500) == 0)
			return 0;
	return (bluetooth_cmd_wait_response()==1); //OK
}

#endif

/**@}*/

