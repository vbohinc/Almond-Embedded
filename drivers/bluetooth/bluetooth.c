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

/**
 * FIFO to store received data. Uses bluetooth_inbuffer to store bytes
 */
fifo_t bluetooth_infifo;

/**
 * Buffer for the FIFO to put received data in.
 */
uint8_t bluetooth_inbuffer[BLUETOOTH_RECEIVE_BUFFER_SIZE];

/**
 * Is set to 1 if bluetooth module is currently connected to a client
 */
uint8_t bluetooth_is_connected = 0;

/**
 * Array to put data package to send into.
 * Also Callback is called with this array to return a received package.
 * This array is also used to return a response of a command.
 * The biggest response is of the ATF? command.
 * The maximum number of found devices is: 8
 * The maximum lenght of a name is: 16
 * The length of an address is: 6 (without '-', each byte contains 2 chars)
 * The data is stored in the following format (where x is the index of found device: Device '1' is at index '0'):
 * arr[0]: Number of found devices
 * arr[1+x*(16+6)] to arr[1+x*(16+6)+15]: Name of found device.
 * 		Ex: arr[1] to arr[15] contains first name terminated by null if shorter than 16 chars
 * arr[1+x*(16+6)+16] to arr[1+x*(16+6)+21]: address of found device.
 * In total there are 1+(16+6)*8=177 bytes needed.
 */
uint8_t bluetooth_data_package[177];

/**
 * Index in the bluetooth_data_package array to store new received byte
 */
uint8_t bluetooth_data_package_index=0;

/**
 * Previous processed byte or -1 if no byte jet received.
 * Used to detect stop byte and masked special bytes
 */
int16_t bluetooth_previous_byte=-1;


/**
 * Size of Buffer to build cmd to send.
 * Should be at least 36 bytes (maximum length of response ATF?
 */
#define BLUETOOTH_CMD_BUFFER_SIZE 36

/**
 * Buffer to build cmd to send or to parse resonse of bluetooth device.
 */
uint8_t bluetooth_cmd_buffer[BLUETOOTH_CMD_BUFFER_SIZE];

/**
 * Contains 3rd and 4th byte of the send command. Ex. for ATF? contains 'F?'.
 * Used to check if received data in online mode is a response of the module to a sent command.
 */
uint8_t bluetooth_cmd_sent[2];

/**
 * Current index to write received byte.
 */
uint8_t bluetooth_cmd_buffer_head = 0;

/**
 * When a response for a command comes in, this variable will be set to the value of the response.
 * @see bluetooth_cmd_wait_response
 */
uint8_t bluetooth_response_code = 0;

uint8_t bluetooth_is_connected;



#ifndef SERIAL
#include <util/delay.h>
#else

#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <unistd.h>
#endif


void bluetooth_init(void (*bluetooth_callback_handler)(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length))
{
	bluetooth_callback = bluetooth_callback_handler;
	fifo_init (&bluetooth_infifo,   bluetooth_inbuffer, BLUETOOTH_RECEIVE_BUFFER_SIZE);
#ifdef SERIAL
	bluetooth_serial_set_byte_handler(bluetooth_byte_received);
	bluetooth_serial_init();
#else
	usart_set_byte_handler(bluetooth_byte_received);
	usart_init();
#endif
}

void bluetooth_close()
{
#ifdef SERIAL
	bluetooth_serial_close();
#endif
}

/**
 * Copy bluetooth from the src array from the format 1234-AB-56789C into dest with the format 1234AB56789C where startIdxSrc is the
 * index where the address starts in the src array and startIdxDest is the start-index to write the address.
 * Arrays must be initialized and destination must be minimum 13 bytes long.
 * @param dest Destination for address
 * @param src Source of the address
 * @param startIdxDest start-index to write address
 * @param startIdxSrc index where the address starts in src
 */
void bluetooth_copy_address(uint8_t* dest, uint8_t* src, const uint8_t startIdxDest, const uint8_t startIdxSrc)
{

	dest[startIdxDest+6] =  0;
}

/**
 * Processes received response from the BTM-222 Module.
 */
void bluetooth_process_response(void)
{
	//received response is empty
	if (bluetooth_cmd_buffer_head == 0)
		return;

	if (strncmp((char*)bluetooth_cmd_buffer, "OK", 2)==0)
	{
		//Special handling for ATF, because OK is only, when Inquiry-End message received
		if (bluetooth_cmd_sent[0]!='F')
			bluetooth_response_code = 1;
		return;
	}

	bluetooth_data_package[0]=0; //Empty return array
	if (strncmp((char*)bluetooth_cmd_buffer, "ERROR", 5)==0)
	{
		bluetooth_response_code = 4;
		return;
	}

	if (strncmp((char*)bluetooth_cmd_buffer, "CONNECT", 7)==0)
	{
		bluetooth_address_to_array(bluetooth_data_package,bluetooth_cmd_buffer, 0, 10, 1);
		bluetooth_is_connected = 1;
		bluetooth_response_code = 2;
		//Call callback function
		bluetooth_callback(bluetooth_data_package, 1, 6);
		return;
	}
	if (strncmp((char*)bluetooth_cmd_buffer, "DISCONNECT", 10)==0)
	{
		bluetooth_address_to_array(bluetooth_data_package,bluetooth_cmd_buffer, 0, 13, 1);
		bluetooth_is_connected =  0;
		bluetooth_response_code = 3;
		//Call callback function
		bluetooth_callback(bluetooth_data_package, 2, 6);
		return;
	}

	switch (bluetooth_cmd_sent[0])
	{
	case 'A':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received timeout error into bluetooth_data_package
		bluetooth_response_code = 4;
		break;
	case 'B':
		//copy received address into return array without '-'
		bluetooth_address_to_array(bluetooth_data_package, bluetooth_cmd_buffer, 0,0, 1);
		break;
	case 'C':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'D':
		//copy received address into return array without '-'
		bluetooth_address_to_array(bluetooth_data_package, bluetooth_cmd_buffer, 0,0, 1);
		break;
	case 'E':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'F':
		if (strncmp((char*)bluetooth_cmd_buffer, "Inquiry End", 11)==0)
		{
			bluetooth_data_package[0] =  bluetooth_cmd_buffer[13]-48; //Get char of count found devices and convert to number
			bluetooth_response_code = 1;
			return;
		}
		if (strncmp((char*)bluetooth_cmd_buffer, "Inquiry Results", 15)==0)
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
		bluetooth_address_to_array(bluetooth_data_package,bluetooth_cmd_buffer, 1+index*(16+6)+16, 21 , 1);

		break;
	case 'H':
		if (bluetooth_cmd_sent[1]==0)
		{ //Connection dropped. Response already handled: OK and DISCONNECT
			return;
		}
		else
			strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	//case 'I':
	//	break;
	//case 'K':
	//	break;
	//case 'L':
	//	break;
	//case 'M':
	//	break;
	case 'N':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received name into return array
		break;
	case 'O':
		if (bluetooth_cmd_sent[1]==0)
		{ //Connection reopened. Response already handled: OK
			return;
		}
		else
			strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'P':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received pin into return array
		break;
	//case 'Q':
	//	break;
	case 'R':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	//case 'U':
	//	break;
	//case 'X':
	//	break;
	//case 'Z':
	//	break;
	case '+':
		break;
	default:
		break;
	}
}

void bluetooth_process_data(void)
{
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
				bluetooth_cmd_buffer_head = 0;
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

			//check if module sent disconnect message
			if (bluetooth_data_package_index > 10 && strstr((char*)bluetooth_data_package, "\r\nDISCONNECT")!=NULL)
			{
				if(byte == 10 && bluetooth_data_package_index==31)
				{ //data was DISCONNECT message from module
					uint8_t i;
					for (i =0 ; i<28; i++)
					{
						bluetooth_cmd_buffer[i] = bluetooth_data_package[i+2];
					}
					bluetooth_cmd_buffer_head = i;
					bluetooth_cmd_buffer[bluetooth_cmd_buffer_head]=0;
					bluetooth_process_response();
					bluetooth_cmd_buffer_head = 0;
					bluetooth_data_package_index=0;
				} else {
					bluetooth_data_package[bluetooth_data_package_index] = (uint8_t)byte;
					bluetooth_previous_byte = byte;
					bluetooth_data_package_index++;
				}
			}
			else if (bluetooth_data_package_index < BLUETOOTH_DATA_PACKAGE_SIZE-1)
			{

				if ((byte == BLUETOOTH_SPECIAL_BYTE) && (bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE))
				{ //Double special byte means that there is one byte in the data package with the value of special byte
					//don't write byte to array
					bluetooth_previous_byte = -1; //to make sure next time this if can't be called
				} else if ((byte==BLUETOOTH_STOP_BYTE) && (bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE))
				{
					//Handle array because last byte was stop byte
					bluetooth_data_package_index--;//don't handle last special byte because is part of stop byte
					bluetooth_handle_array();
					bluetooth_data_package_index=0;
				} else {
					bluetooth_data_package[bluetooth_data_package_index] = (uint8_t)byte;
					bluetooth_previous_byte = byte;
					bluetooth_data_package_index++;
				}
				if (bluetooth_data_package_index >= BLUETOOTH_DATA_PACKAGE_SIZE)
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

/*
	//if array handling didn't finished jet, don't call again.
	//After finished handling is another check, if data is available.
	if (bluetooth_receiveArray_handling==1)
		return;

	bluetooth_receiveArray_handling=1;
	bluetooth_process_data();
	bluetooth_receiveArray_handling=0;
*/
}

void bluetooth_handle_array(void)
{
	if (bluetooth_data_package_index<9)
	{
		printf("Package too small!!!\n");
		return;
	}
	/*
	 * Calculate and check if CRC32 checksum is ok.
	 * The last 8 bytes of package should be the reversed crc value
	 */
	uint32_t checksum = 0xFFFFFFFF;
	crc_32(&checksum, bluetooth_data_package, bluetooth_data_package_index-8);
	checksum ^= 0xffffffff;

	//Check checksum reversed
	for (int8_t i=7; i>=0; i--)
	{
		if ((checksum&0xF) != bluetooth_data_package[bluetooth_data_package_index-1-i])
		{
			printf("CRC ERROR!!!\n");
			return;
		}
		checksum = checksum >> 4;
	}

	//Overwrite checksum with 0
	for (uint8_t i=bluetooth_data_package_index-8; i<bluetooth_data_package_index;i++)
		bluetooth_data_package[i]=0;

	//Call callback function
	bluetooth_callback(bluetooth_data_package, 0, bluetooth_data_package_index-8);
}

uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length)
{
	bluetooth_cmd_buffer[0]=0;
	uint8_t error = 0;
	for (uint8_t i=0; i<length && error==0; i++)
	{
#ifdef SERIAL
		if (data[i]==BLUETOOTH_SPECIAL_BYTE)
		{
			error = (error || (bluetooth_serial_putc(BLUETOOTH_SPECIAL_BYTE)==0));
			//There must be a sleep otherwise the module returns always error
			usleep(10000);
		/*	struct timespec s;
			s.tv_sec = 0;
			s.tv_nsec = 10000000L;
			nanosleep(&s, NULL);*/
		}
		error = (error || (bluetooth_serial_putc(data[i])==0));
		//There must be a sleep otherwise the module returns always error
		usleep(10000);
		/*struct timespec s;
		s.tv_sec = 0;
		s.tv_nsec = 10000000L;
		nanosleep(&s, NULL);*/
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

	/*
	 * Calculate and send CRC32 checksum
	 */
	uint32_t checksum = 0xFFFFFFFF;
	crc_32(&checksum, data, length);
	checksum ^= 0xffffffff;

	//Send checksum reversed
	for (int8_t i=7; i>=0; i--)
	{
#ifdef SERIAL
		//send crc byte
		error = (error || (bluetooth_serial_putc(checksum&0xF)==0));

		usleep(10000);
#else
		//send crc byte
		error = (error || (usart_putc(checksum&0xF,  10)==0));
		/*! @todo optimize delay */
		_delay_ms(10);
#endif
		checksum = checksum >> 4;
	}

#ifdef SERIAL
	//send stop byte
	error = (error || (bluetooth_serial_putc(BLUETOOTH_SPECIAL_BYTE)==0));

	usleep(10000);
	error = (error || (bluetooth_serial_putc(BLUETOOTH_STOP_BYTE)==0));

#else
	//send stop byte
	error = (error || (usart_putc(BLUETOOTH_SPECIAL_BYTE,  10)==0));
	/*! @todo optimize delay */
	_delay_ms(10);
	error = (error || (usart_putc(BLUETOOTH_STOP_BYTE,  10)==0));
#endif

	return (error==0);
}

uint8_t bluetooth_set_as_master(void)
{
	uint8_t* currentMode = bluetooth_cmd_get_mode();
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
	uint8_t* currentMode = bluetooth_cmd_get_mode();
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
uint8_t char_to_hex(uint8_t chr)
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
uint8_t hex_to_char(uint8_t hex)
{
	if (hex<=9)
		return hex+'0';
	else if (hex>9 && hex <= 15)
		return hex+'A'-10;
	else
		return 0;
}


void bluetooth_address_to_array(uint8_t *full_address, uint8_t *compressed_address, const uint8_t full_start_idx, const uint8_t compressed_start_idx, const uint8_t address_with_hyphen)
{
	uint8_t addr_buffer = 0;
	uint8_t buf_idx = full_start_idx;

	for (uint8_t i=compressed_start_idx; i<compressed_start_idx+6; i++)
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

void bluetooth_array_to_address(uint8_t *compressed_address, uint8_t *full_address, const uint8_t compressed_start_idx, const uint8_t full_start_idx, const uint8_t address_with_hyphen)
{

	uint8_t addr_buffer = 0;
	uint8_t buf_idx = full_start_idx;

	for (uint8_t i=compressed_start_idx; i<compressed_start_idx+6; i++)
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


uint8_t bluetooth_cmd_send (const uint8_t* cmd, const uint16_t delay_ms)
{
	bluetooth_response_code = 0;


	uint8_t command_length = 0;


	bluetooth_cmd_sent[0] = cmd[2];
	bluetooth_cmd_sent[1] = cmd[3];

#ifdef SERIAL
	printf("\nCMD:'%s'\n", cmd);

	while (cmd[command_length]!=0)
	{
		if (bluetooth_serial_putc(cmd[command_length]) == 0)
			return 0;

		//There must be a sleep otherwise the module returns always error
		/*struct timespec s;
		s.tv_sec = 0;
		//don't make it tooo small otherwise the module dosen't return correct values
		s.tv_nsec = delay_ms * 1000000L;
		nanosleep(&s, NULL);*/
		usleep(delay_ms * 1000);


		command_length++;
	}
#else
	while (cmd[command_length]!=0)
	{
		if (usart_putc(cmd[command_length], 10) == 0)
			return 0;
		_delay_ms(delay_ms);
		command_length++;
	}
#endif

	#ifdef SERIAL
		usleep(500000); //Wait until device has finished command
	#else
		_delay_ms(500); //Wait until device has finished command
	#endif

	return 1;
}

uint8_t bluetooth_cmd_wait_response (void)
{
	while (bluetooth_response_code == 0) //Wait until bluetooth devices has responsed (handled by interrupt)
	{
		bluetooth_process_data();
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
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1)//OK
		return 0;

	bluetooth_response_code = 0;

	return (bluetooth_cmd_wait_response()==2); //CONNECT
}

uint8_t bluetooth_cmd_test_connection (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 13; //<CR>
	bluetooth_cmd_buffer[3] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
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


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}

uint8_t bluetooth_cmd_set_remote_address (const uint8_t* address)
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
		bluetooth_cmd_buffer[3]='=';

		bluetooth_array_to_address((uint8_t*)address,bluetooth_cmd_buffer,0,4,0);

		bluetooth_cmd_buffer[16] = 13; //<CR>
		bluetooth_cmd_buffer[17] = 0;

	}

	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()==1) //OK
	{
		#ifdef SERIAL
			usleep(2000000); //Wait until device has finished command
		#else
			_delay_ms(2000); //Wait until device has finished command
		#endif
		return 1;
	} else
		return 0;
}

uint8_t* bluetooth_cmd_search_devices (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'F';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}

uint8_t bluetooth_cmd_close_connection (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'H';
	bluetooth_cmd_buffer[3] = 13; //<CR>
	bluetooth_cmd_buffer[4] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK
}

uint8_t bluetooth_cmd_discoverable (const uint8_t discoverable)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'H';
	bluetooth_cmd_buffer[3] = discoverable+48; //Convert number to number as char
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK
}

uint8_t bluetooth_cmd_set_name (const uint8_t *name)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'N';
	bluetooth_cmd_buffer[3] = '=';
	if (name == NULL)
		return 0;

	uint8_t i;
	for (i=0; i<16 && name[i]!=0; i++)
	{
		bluetooth_cmd_buffer[i+4] = name[i];
	}
	bluetooth_cmd_buffer[i+4] = 13; //<CR>
	bluetooth_cmd_buffer[i+5] = 0;

	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
			return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

uint8_t* bluetooth_cmd_get_name (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'N';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;

}

uint8_t bluetooth_cmd_autoconnect (const uint8_t autoconnect)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'O';
	if (autoconnect == 1)
		bluetooth_cmd_buffer[3] = '0';
	else
		bluetooth_cmd_buffer[3] = '1';

	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()==1) //OK
	{
		#ifdef SERIAL
			usleep(3000000); //Wait until device has finished warm start
		#else
			_delay_ms(3000); //Wait until device has finished warm start
		#endif
		return 1;
	} else
		return 0;
}

uint8_t bluetooth_cmd_set_pin (const uint8_t *pin)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'P';
	if (pin == NULL)
		return 0;

	uint8_t i;
	for (i=0; i<16 && pin[i]!=0; i++)
	{

		bluetooth_cmd_buffer[3] = '=';
		bluetooth_cmd_buffer[i+4] = pin[i];
	}
	bluetooth_cmd_buffer[i+4] = 13; //<CR>
	bluetooth_cmd_buffer[i+5] = 0;

	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
			return 0;

	return (bluetooth_cmd_wait_response()==1); //OK

}

uint8_t* bluetooth_cmd_get_pin (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'P';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;

}

uint8_t bluetooth_cmd_set_mode (uint8_t mode)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'R';
	bluetooth_cmd_buffer[3] = mode+48; //Convert number to number as char
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;
	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()==1) //OK
	{
		#ifdef SERIAL
			usleep(3500000); //Wait until device has finished warm start
		#else
			_delay_ms(3500); //Wait until device has finished warm start
		#endif
		return 1;
	} else
		return 0;

}

uint8_t* bluetooth_cmd_get_mode (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'R';
	bluetooth_cmd_buffer[3] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0)
		return NULL;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_data_package;
}

uint8_t bluetooth_cmd_restore_settings (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'Z';
	bluetooth_cmd_buffer[3] = '0';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;


	return (bluetooth_cmd_send(bluetooth_cmd_buffer, BLUETOOTH_CMD_WAIT_TIME) == 0);

}

uint8_t bluetooth_cmd_online_command (void)
{
	bluetooth_cmd_buffer[0] = '+';
	bluetooth_cmd_buffer[1] = '+';
	bluetooth_cmd_buffer[2] = '+';
	bluetooth_cmd_buffer[3] = '+';//write four '+'. Only three needed, but sometimes one isn't recognized
	bluetooth_cmd_buffer[4] = 0;


	if (bluetooth_cmd_send(bluetooth_cmd_buffer, 1500) == 0)
			return 0;
	return (bluetooth_cmd_wait_response()==1); //OK
}


