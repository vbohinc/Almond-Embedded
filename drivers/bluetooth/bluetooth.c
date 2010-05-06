/**
* @file bluetooth.c
* @author Stefan Profanter
* @date 26.04.2010
*
* This file contanis the bodys for the functions available to
* communicate with the Bluetooth Module over USART.
* Used Bluetooth Module: BTM-222
*
*/

#include "bluetooth.h"
#include <string.h>

//uint8_t bluetooth_receiveArray[BLUETOOTH_RECEIVE_BUFFER_ARRAYS][BLUETOOTH_RECEIVE_BUFFER_SIZE];

/*! Current position to write a new byte in the receiveArray */
//uint8_t bluetooth_receiveArray_head = 0;

/**
 * FIFO to store received data.
 */
fifo_t bluetooth_infifo;

/*! Buffer for the FIFO to put received data in. */
uint8_t bluetooth_inbuffer[BLUETOOTH_RECEIVE_BUFFER_SIZE];

/*! 'Semaphore' to check, if callback-handler is currently processing array or not*/
uint8_t bluetooth_receiveArray_handling = 0;


uint8_t bluetooth_is_connected = 0;

/* Change also in .h file the size!! */
uint8_t bluetooth_data_package[225];

/*! Index in the bluetooth_data_package array to store new byte */
uint8_t bluetooth_data_package_index=0;

/*! Previous processed byte or -1 if no byte jet received*/
int16_t bluetooth_previous_byte=-1;


/*! Stores which arrays are available to store received data */
//uint8_t bluetooth_receiveArray_handled[BLUETOOTH_RECEIVE_BUFFER_ARRAYS];

/*! Current array to write received data  */
//uint8_t bluetooth_receiveArray_current = 0;

/*! If all arrays are full this variable will be set to USART_RECEIVE_BUFFER_SIZE because the next USART_RECEIVE_BUFFER_SIZE bytes have to be discarded */
//uint8_t bluetooth_receiveArray_bytes_to_discard = 0;


/*! Size of Buffer to build cmd to send.
 * Should be at least 31 bytes (maximum length of response ATF?
 */
#define BLUETOOTH_CMD_BUFFER_SIZE 31

/*! Buffer to build cmd to send or to parse resonse of bluetooth device.
 */
uint8_t bluetooth_cmd_buffer[BLUETOOTH_CMD_BUFFER_SIZE];

/*! Contains 3rd and 4th byte of the send command. Ex. for ATF? contains 'F?'
 */
uint8_t bluetooth_cmd_sent[2];

/*! Current index to write received byte
 */
uint8_t bluetooth_cmd_buffer_head = 0;

/*! When a response for a command comes in, this variable will be set to the value of the response.
 * @see bluetooth_cmd_wait_response
 */
uint8_t bluetooth_response_code = 0;



#ifndef SERIAL
#include <util/delay.h>
#else

#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


void bluetooth_init()
{
	fifo_init (&bluetooth_infifo,   bluetooth_inbuffer, BLUETOOTH_RECEIVE_BUFFER_SIZE);
#ifdef SERIAL
	bluetooth_serial_set_byte_handler(bluetooth_byte_received);
	bluetooth_serial_init();
#else
	usart_set_byte_handler(bluetooth_byte_received);
	usart_init();
#endif
	//Make all receiveBuffer arrays available to store data.
	/*for (uint8_t i=0; i<BLUETOOTH_RECEIVE_BUFFER_ARRAYS; i++)
	{
		bluetooth_receiveArray_handled[i] = 1;
	}*/
}

void bluetooth_close()
{
#ifdef SERIAL
	bluetooth_serial_close();
#endif
}

void bluetooth_process_response(void)
{
	if (bluetooth_cmd_buffer_head == 0)
		return;
	if (strncmp((char*)bluetooth_cmd_buffer, "OK", 2)==0)
	{
		bluetooth_response_code = 1;
		return;
	}
	if (strncmp((char*)bluetooth_cmd_buffer, "ERROR", 5)==0)
	{
		bluetooth_response_code = 2;
		return;
	}

	switch (bluetooth_cmd_sent[0])
	{
	case 'A':
		break;
	case 'B':
		strcpy((char*)bluetooth_cmd_buffer,(char*)bluetooth_data_package); //copy received address into return array
		break;
	case 'C':
		strcpy((char*)bluetooth_cmd_buffer,(char*)bluetooth_data_package); //copy received value into return array
		break;
	case 'D':
		strcpy((char*)bluetooth_cmd_buffer,(char*)bluetooth_data_package); //copy received address into return array
		break;
	case 'E':
		strcpy((char*)bluetooth_cmd_buffer,(char*)bluetooth_data_package); //copy received value into return array
		break;
	case 'F':
		//Kompliziertes gefummel!!
		break;
	case 'H':
		if (bluetooth_cmd_sent[1]==0)
		{ //

		}
		else
			strcpy((char*)bluetooth_cmd_buffer,(char*)bluetooth_data_package); //copy received value into return array
		break;
	case 'I':
		break;
	case 'K':
		break;
	case 'L':
		break;
	case 'M':
		break;
	case 'N':
		break;
	case 'O':
		break;
	case 'P':
		break;
	case 'Q':
		break;
	case 'R':
		break;
	case 'U':
		break;
	case 'X':
		break;
	case 'Z':
		break;
	case '+':
		break;
	default:
		break;
	}
}

/**
 * Processes received data stored in the FIFO.
 */
void bluetooth_process_data(void)
{
	while (bluetooth_infifo.count>0)
	{ //read all bytes from fifo

		if (bluetooth_is_connected==0)
		{
			//Data in FIFO is response to a sent command
			int16_t byte = fifo_get_nowait(&bluetooth_infifo);
#ifdef SERIAL
			if ((byte >= 40 && byte <=62) ||  (byte >= 65 && byte <=90) ||  (byte >= 97 && byte <=122))
				printf("%c", byte);
			else if (byte == 13)
				printf("<CR>\n");
			else if (byte == 10)
				printf("<LF>");
			else
				printf("[%d]",byte);
			fflush(stdout);
#endif


			if (byte == 10)
			{
				//handle received command

				bluetooth_cmd_buffer[bluetooth_cmd_buffer_head]=0;
				bluetooth_process_response();
			} else if (byte != 13) //don't handle <CR>
			{
				bluetooth_cmd_buffer[bluetooth_cmd_buffer_head] = byte;
				bluetooth_cmd_buffer_head++;
				if (bluetooth_cmd_buffer_head == BLUETOOTH_CMD_BUFFER_SIZE)
				{
					bluetooth_cmd_buffer_head = 0;
					bluetooth_response_code = 5; //No valid response
				}
			}

		} else {
			//Data in FIFO is a data package for the callback function
			int16_t byte = fifo_get_nowait(&bluetooth_infifo);
			if (byte == -1)
				return;

			if (bluetooth_data_package_index < BLUETOOTH_DATA_PACKAGE_SIZE-1)
			{

				if ((byte == BLUETOOTH_SPECIAL_BYTE) && (bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE))
				{ //Double special byte means that there is one byte in the data package with the value of special byte
					bluetooth_previous_byte = -1; //to make sure next time this if can't be called
					bluetooth_data_package[bluetooth_data_package_index] = (uint8_t)byte;
					bluetooth_data_package_index++;
				} else if ((byte==BLUETOOTH_STOP_BYTE) && (bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE))
				{
					//Handle array because last byte was stop byte
					bluetooth_handle_array();
					bluetooth_data_package_index=0;
				} else {
					bluetooth_data_package[bluetooth_data_package_index] = (uint8_t)byte;
					bluetooth_previous_byte = byte;
					bluetooth_data_package_index++;
				}
				if (bluetooth_data_package_index < BLUETOOTH_DATA_PACKAGE_SIZE-1)
				{
					//array is full, handle it
					bluetooth_handle_array();
					bluetooth_data_package_index=0;
				}
			} else {
				//Handle array because it's full
				bluetooth_handle_array();
				bluetooth_data_package_index=0;
			}
		}
	}
}

void bluetooth_byte_received (uint8_t byte)
{
	//Put received byte into fifo
	_inline_fifo_put (&bluetooth_infifo, byte);

	//if array handling didn't finished jet, don't call again.
	//After finished handling is another check, if data is available.
	if (bluetooth_receiveArray_handling==1)
		return;

	bluetooth_receiveArray_handling=1;
	bluetooth_process_data();
	bluetooth_receiveArray_handling=0;
}

void bluetooth_handle_array(void)
{
	//Fill empty indexes with 0 (there shouldn't be any!!)
	for (uint8_t i=bluetooth_data_package_index; i<BLUETOOTH_DATA_PACKAGE_SIZE;i++)
		bluetooth_data_package[i]=0;

	//Call callback function
}

uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length)
{
	uint8_t error = 0;
	for (uint8_t i=0; i<length && error==0; i++)
	{
#ifdef SERIAL
		if (data[i]==BLUETOOTH_SPECIAL_BYTE)
		{
			error = (error || (bluetooth_serial_putc(BLUETOOTH_SPECIAL_BYTE)==0));
			//There must be a sleep otherwise the module returns always error
			struct timespec s;
			s.tv_sec = 0;
			s.tv_nsec = 10000000L;
			nanosleep(&s, NULL);
		}
		error = (error || (bluetooth_serial_putc(data[i])==0));
		//There must be a sleep otherwise the module returns always error
		struct timespec s;
		s.tv_sec = 0;
		s.tv_nsec = 10000000L;
		nanosleep(&s, NULL);
#else
		if (data[i]==BLUETOOTH_SPECIAL_BYTE)
		{
			error = (error || (usart_putc(BLUETOOTH_SPECIAL_BYTE,  10)==0));
			/*! @TODO optimize delay */
			_delay_ms(10);
		}
		error = (error || (usart_putc(data[i],  10)==0));

		/*! @TODO optimize delay */
		_delay_ms(10);
#endif

	}

#ifdef SERIAL
	//send stop byte
	error = (error || (bluetooth_serial_putc(BLUETOOTH_SPECIAL_BYTE)==0));

	struct timespec s;
	s.tv_sec = 0;
	s.tv_nsec = 10000000L;
	nanosleep(&s, NULL);
	error = (error || (bluetooth_serial_putc(BLUETOOTH_STOP_BYTE)==0));

#else
	//send stop byte
	error = (error || (usart_putc(BLUETOOTH_SPECIAL_BYTE,  10)==0));
	/*! @TODO optimize delay */
	_delay_ms(10);
	error = (error || (usart_putc(BLUETOOTH_STOP_BYTE,  10)==0));
#endif
	return (error==0);
}



//---------------------------------------------------------
//
//               Bluetooth Commands
//
//---------------------------------------------------------


uint8_t bluetooth_cmd_send (uint8_t* cmd, const uint16_t delay_ms)
{
	bluetooth_response_code = 0;


	uint8_t command_length = 0;


	bluetooth_cmd_sent[0] = cmd[2];
	bluetooth_cmd_sent[1] = cmd[3];

#ifdef SERIAL
	while (cmd[command_length]!=0)
		command_length++;
	if (bluetooth_serial_send_bytes(cmd, command_length) == 0)
		return 0;
#else
	while (cmd[command_length]!=0)
	{
		if (usart_putc(cmd[command_length], 10) == 0)
			return 0;
		_delay_ms(delay_ms);
		command_length++;
	}
#endif

	return 1;
}

uint8_t bluetooth_cmd_wait_response (void)
{
	while (bluetooth_response_code == 0) //Wait until bluetooth devices has responsed (handled by interrupt)
	{
#ifdef SERIAL
		struct timespec s;
		s.tv_sec = 0;
		s.tv_nsec = 100000L;
		nanosleep(&s, NULL);
#endif
	}
	return bluetooth_response_code;
}

uint8_t bluetooth_cmd_connect (const uint8_t dev_num)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'A';
	if (dev_num>0)
	{
		bluetooth_cmd_buffer[3] = dev_num+48; //Convert number to number as char
		bluetooth_cmd_buffer[4] = 13; //<CR>
		bluetooth_cmd_buffer[5] = 0;
	} else {
		bluetooth_cmd_buffer[3] = 13; //<CR>
		bluetooth_cmd_buffer[4] = 0;
	}
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==2); //CONNECT
}

uint8_t bluetooth_cmd_test_connection (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 13; //<CR>
	bluetooth_cmd_buffer[3] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

uint8_t* bluetooth_cmd_get_address (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'B';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}

uint8_t bluetooth_cmd_set_remote_address (uint8_t* address)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'D';
	if (address == NULL)
	{
		bluetooth_cmd_buffer[3] = '0'; //Convert number to number as char
		bluetooth_cmd_buffer[4] = 13; //<CR>
		bluetooth_cmd_buffer[5] = 0;
	} else {
		uint8_t i;
		for (i=0; i<12; i++)
		{
			bluetooth_cmd_buffer[i+3] = address[i];
		}
		bluetooth_cmd_buffer[i] = 13; //<CR>
		bluetooth_cmd_buffer[i+1] = 0;

	}

	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK
}

uint8_t* bluetooth_cmd_search_devices (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'F';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}


uint8_t bluetooth_cmd_set_name (uint8_t *name)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'N';
	if (name == NULL)
		return 0;

	uint8_t i;
	for (i=0; i<16 && name[i]!=0; i++)
	{
		bluetooth_cmd_buffer[i+3] = name[i];
	}
	bluetooth_cmd_buffer[i] = 13; //<CR>
	bluetooth_cmd_buffer[i+1] = 0;

	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
			return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

uint8_t* bluetooth_cmd_get_name (uint8_t *name)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'N';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;

}


uint8_t bluetooth_cmd_set_pin (uint8_t *pin)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'P';
	if (pin == NULL)
		return 0;

	uint8_t i;
	for (i=0; i<16 && pin[i]!=0; i++)
	{
		bluetooth_cmd_buffer[i+3] = pin[i];
	}
	bluetooth_cmd_buffer[i] = 13; //<CR>
	bluetooth_cmd_buffer[i+1] = 0;

	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
			return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

uint8_t bluetooth_cmd_set_mode (uint8_t mode)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'R';
	bluetooth_cmd_buffer[3] = mode+48; //Convert number to number as char
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

uint8_t bluetooth_cmd_restore_settings (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'Z';
	bluetooth_cmd_buffer[3] = '0';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	return (bluetooth_cmd_send(bluetooth_cmd_buffer, 10) == 0);

}


