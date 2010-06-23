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
#include <stdlib.h>
#include "ftdi.h"

#include <avr/eeprom.h>

#ifdef SERIAL
#include <unistd.h>
#endif

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



uint8_t put_count = 0;

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
 *
 * The ATF? command is only available on Squirrel to save memory.
 * On Nut the maximum size is approx. the DISCONNECT response
 */
#ifdef SQUIRREL
uint8_t bluetooth_data_package[177];
#else
uint8_t bluetooth_data_package[35];
#endif

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
 * Should be at least 36 bytes (maximum length of one line of response ATF? )
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

/**
 * Contains the length of the recently sent package. The data is stored in bluetooth_data_package
 */
uint8_t bluetooth_sent_length = 0;

/**
 * Array to copy response package into. If NULL, no response array expected
 */
uint8_t *bluetooth_wait_response_array = NULL;

/**
 * Pointer to copy response package length into. If NULL, no response array expected
 */
uint8_t *bluetooth_wait_response_length = NULL;

/**
 * timeout to wait for stop byte. In ms
 */
#define BLUETOOTH_STOP_BYTE_TIMEOUT 1000

/**
 * when new data package begin recieved, bluetooth_base_stop_reading will be set to STOP_BYTE_TIMEOUT.
 * If no bytes received for STOP_BYTE_TIMEOUT ms, a timeout occurs and package will be requested because there was no stop byte or there was a transmission error.
 */
int32_t bluetooth_ms_to_timeout = -1; //disable timeout


/**
 * Used for the bluetooth_send_datapackage to determine if remote end responsed with a package
 */
uint8_t bluetooth_package_received = 0;

#ifdef BLUETOOTH_ENABLE_OK
/**
 * Used for the bluetooth_send_datapackage to determine if remote end responsed with ok
 */
uint8_t bluetooth_ok_received = 0;
#endif

#define UART_BAUD_RATE      19200


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
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
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

uint8_t my_strncmp_eeprom(const uint8_t *str1, const uint8_t *str2, const uint8_t count)
{
	uint8_t  myByte;

	uint8_t i = 0;
	do
	{
		myByte = eeprom_read_byte(str2+i);
		if (myByte != '\0')
		{
			if (str1[i] != myByte)
				return 1;
		}
		else //myByte is '\0'
			break;
		i++;
	} while (i<count && str1[i]!='\0' && myByte!='\0');
	return 0;
}

/**
 * Processes received response from the BTM-222 Module.
 */
void bluetooth_process_response(void)
{
	//received response is empty
	if (bluetooth_cmd_buffer_head == 0)
		return;
	if (my_strncmp_eeprom(bluetooth_cmd_buffer, str_ok, 2)==0)
	{
		//Special handling for ATF, because OK is only, when Inquiry-End message received
		if (bluetooth_cmd_sent[0]!='F')
			bluetooth_response_code = 1;
		return;
	}
	bluetooth_data_package[0]=0; //Empty return array
	if (my_strncmp_eeprom(bluetooth_cmd_buffer, str_error, 5)==0)
	{
		bluetooth_response_code = 4;
		return;
	}
	if (my_strncmp_eeprom(bluetooth_cmd_buffer, str_connect, 7)==0)
	{
		bluetooth_address_to_array(bluetooth_cmd_buffer, bluetooth_data_package, 10, 0, 1);
		bluetooth_is_connected = 1;
		bluetooth_response_code = 2;
		//Call callback function
		bluetooth_callback(bluetooth_data_package, 1, 6);
		return;
	}
	if (my_strncmp_eeprom(bluetooth_cmd_buffer, str_disconnect, 10)==0)
	{
		bluetooth_address_to_array(bluetooth_cmd_buffer,bluetooth_data_package, 13, 0, 1);
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
		bluetooth_address_to_array(bluetooth_cmd_buffer,bluetooth_data_package , 0,0, 1);
		break;
	case 'C':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'D':
		//copy received address into return array without '-'
		bluetooth_address_to_array(bluetooth_cmd_buffer,bluetooth_data_package,  0,0, 1);
		break;
	case 'E':
		strcpy((char*)bluetooth_data_package,(char*)bluetooth_cmd_buffer); //copy received value into return array
		break;
	case 'F':
		if (my_strncmp_eeprom(bluetooth_cmd_buffer, str_inquiry_end, 11)==0)
		{
			bluetooth_data_package[0] =  bluetooth_cmd_buffer[13]-48; //Get char of count found devices and convert to number
			bluetooth_response_code = 1;
			return;
		}
		if (my_strncmp_eeprom(bluetooth_cmd_buffer, str_inquiry_results, 15)==0)
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
		bluetooth_address_to_array(bluetooth_cmd_buffer, bluetooth_data_package,1+index*(16+6)+16, 21 , 1);

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

/**
 * Special delay function to distinguish between Microcontroller Mode or
 * PC Mode
 */
void bluetooth_delay(uint16_t ms)
{
#ifdef SERIAL
	int sec = ms/1000;
	ms -= sec*1000;

	struct timespec s;
	s.tv_sec = sec;
	s.tv_nsec = 1000000L * ms;
	nanosleep(&s, NULL);
#else
	_delay_ms(ms);
#endif
}

int inline bluetooth_putc(const uint8_t byte)
{
	/*if (byte == 13)
		debug("BTM: Putc: '<CR>' [13]");
	else if (byte == 10)
		debug("BTM: Putc: '<LF>' [10]");
	else
	{
		debug("BTM: Putc: ");
		error_builder[0] = byte;
		error_builder[1] = 0;
		debug(error_builder);
		debug(itoa(byte, error_builder,10));
	}*/

	uint8_t error = 0;
#ifdef SERIAL
		error = (error || (bluetooth_serial_putc(byte)==0));

#else
		uart_putc(byte);
#endif
	return !error;
}

/**
 * Resents the byte from bluetooth_data_package because the remote end asked to do so (caused by a crc error)
 */
void bluetooth_resent_package(void)
{
	debug_eeprom(str_bt_resent_pkg);
	for (uint8_t i=0; i<bluetooth_sent_length; i++)
	{
		if (bluetooth_putc(bluetooth_data_package[i])==0)
			break;
	}
}

void bluetooth_input_to_array(void)
{
#ifndef SERIAL
	//Read bytes from UART input buffer
	unsigned int c;

	do
	{
		c =  uart_getc();
		if ( c & UART_FRAME_ERROR )
		{
			/* Framing Error detected, i.e no stop bit detected */
			warn_eeprom(str_uart_frame_error);
		}
		if ( c & UART_OVERRUN_ERROR )
		{
			/*
			 * Overrun, a character already present in the UART UDR register was
			 * not read by the interrupt handler before the next character arrived,
			 * one or more received characters have been dropped
			 */
			warn_eeprom(str_uart_overrun_error);
		}
		if ( c & UART_BUFFER_OVERFLOW )
		{
			/*
			 * We are not reading the receive buffer fast enough,
			 * one or more received character have been dropped
			 */
			warn_eeprom(str_uart_buffer_overflow);
		}
		else if (c!=UART_NO_DATA)
		{
			_inline_fifo_put (&bluetooth_infifo, c);
		}

	} while (c!=UART_NO_DATA && c!= UART_FRAME_ERROR && c!= UART_OVERRUN_ERROR && c!=UART_BUFFER_OVERFLOW);

#endif
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
			//FTDISend(byte);

		/*	if (byte == 13)
				debug("BTM:fifo_get:'<CR>' [13]");
			else if (byte == 10)
				debug("BTM: fifo_get:'<LF>' [10]");
			else
			{
				debug("BTM: fifo_get:");
				error_builder[0] = byte;
				error_builder[1] = 0;
				debug(error_builder);
				debug(itoa(byte, error_builder,10));
			}*/
			if (byte == 10)
			{
				bluetooth_input_to_array();
				while (bluetooth_infifo.count>0)
				{
					fifo_get_nowait(&bluetooth_infifo);
				}
				//handle received command
				bluetooth_cmd_buffer[bluetooth_cmd_buffer_head]=0;
				bluetooth_process_response();
				bluetooth_cmd_buffer_head = 0;
			} else if (byte != 13 && byte != 0) //don't handle <CR>
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

			//will be broken when package is complete or full
			while (1)
			{
				bluetooth_input_to_array();
				//Data in FIFO is a data package for the callback function
				int16_t byte = fifo_get_nowait(&bluetooth_infifo);

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

							debug_eeprom(str_bt_timeout_stop);

							//reorder package
							bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
							bluetooth_putc(BLUETOOTH_RESENT_BYTE);
						}

						bluetooth_delay(1);
					} else {
						return;
					}
				}
				else
				{
					//debug("BTM: fifo_get:");
					//debug(itoa(byte, error_builder,10));
					//printf("recByte=%d\n", byte);
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

							if (bluetooth_handle_array() == -1)
							{
								//flush fifo
								while (bluetooth_infifo.count>0)
									fifo_get_nowait(&bluetooth_infifo);
							}
							bluetooth_data_package_index=0;
							break;
						}
						else if (byte == BLUETOOTH_RESENT_BYTE  && bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE)
						{
							debug_eeprom(str_bt_req_resent);
							bluetooth_ms_to_timeout = -1; //disable timeout
							bluetooth_previous_byte = -1;

							bluetooth_data_package_index=0;
							bluetooth_resent_package();
							break;
						}
#ifdef BLUETOOTH_ENABLE_OK
						else if (byte == BLUETOOTH_OK_BYTE  && bluetooth_previous_byte==BLUETOOTH_SPECIAL_BYTE)
						{
							bluetooth_previous_byte = -1;
							bluetooth_ms_to_timeout = BLUETOOTH_STOP_BYTE_TIMEOUT; //reset timeout

							bluetooth_data_package_index=0;
							bluetooth_ok_received = 1;
							if (bluetooth_wait_response_array == NULL)
							{
								//flush fifo
								while (bluetooth_infifo.count>0)
									fifo_get_nowait(&bluetooth_infifo);
								break;
							}
						}
#endif
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
								if (bluetooth_handle_array() == -1)
								{
									//flush fifo
									while (bluetooth_infifo.count>0)
										fifo_get_nowait(&bluetooth_infifo);
								}
								bluetooth_data_package_index=0;
								break;
							}
						} else {
							debug_eeprom(str_bt_pck_full);
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

/*
void bluetooth_byte_received (uint8_t byte)
{*/
	//Put received byte into fifo
//	_inline_fifo_put (&bluetooth_infifo, byte);

	//printf("Byte: %d\n", byte);
	//fflush(stdout);

/*
	//if array handling didn't finished jet, don't call again.
	//After finished handling is another check, if data is available.
	if (bluetooth_receiveArray_handling==1)
		return;

	bluetooth_receiveArray_handling=1;
	bluetooth_process_data();
	bluetooth_receiveArray_handling=0;
*/
//}

uint8_t bluetooth_handle_array(void)
{

#ifdef ENABLE_CRC
	if (bluetooth_data_package_index<9)
	{
		debug_eeprom(str_bt_pck_small);
		bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
		bluetooth_putc(BLUETOOTH_RESENT_BYTE);
		return -1;
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

			debug_eeprom(str_bt_crc_error);

			//Reorder package
			bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
			bluetooth_putc(BLUETOOTH_RESENT_BYTE);

			return -1;
		}
		checksum = checksum >> 4;
	}

	//Overwrite checksum with 0
	//for (uint8_t i=bluetooth_data_package_index-8; i<bluetooth_data_package_index;i++)
	//	bluetooth_data_package[i]=0;

	bluetooth_data_package_index-=8;
#endif


#ifdef BLUETOOTH_ENABLE_OK

	for (uint8_t i = 0; i<1; i++)
	{
		//send ok byte
		bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
		bluetooth_putc(BLUETOOTH_OK_BYTE);
	}
#endif

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




	return 0;
}

uint8_t bluetooth_send_data_package(uint8_t *data, uint8_t *length, const uint8_t wait_for_response_package, const uint16_t timeout_ms)
{
	put_count = 0;
	bluetooth_cmd_buffer[0]=0;
	uint8_t error = 0;

	//send data and store send bytes to bluetooth_data_package to resent full package if requested
	bluetooth_data_package_index = 0;

	for (uint8_t i=0; i<(*length) && error==0; i++)
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
		put_count++;
	}

#ifdef ENABLE_CRC
	/*
	 * Calculate and send CRC32 checksum
	 */
	uint32_t checksum = 0xFFFFFFFF;
	crc_32(&checksum, data, *length);
	checksum ^= 0xffffffff;

	//Send checksum reversed
	for (int8_t i=7; i>=0; i--)
	{
		//send crc byte
		error = (error || (bluetooth_putc(checksum&0xF)==0));
		put_count++;

		bluetooth_data_package[bluetooth_data_package_index] = (checksum&0xF);
		bluetooth_data_package_index++;
		checksum = checksum >> 4;
	}
#endif

	bluetooth_package_received = 0;


	if (wait_for_response_package)
	{
		bluetooth_wait_response_array = data;
		bluetooth_wait_response_length = length;

	} else {
		bluetooth_wait_response_array = NULL;
		bluetooth_wait_response_length = NULL;
	}

	bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
	put_count++;
	bluetooth_putc(BLUETOOTH_STOP_BYTE);
	put_count++;


	bluetooth_data_package[bluetooth_data_package_index] = BLUETOOTH_SPECIAL_BYTE;
	bluetooth_data_package_index++;
	bluetooth_data_package[bluetooth_data_package_index] = BLUETOOTH_STOP_BYTE;
	bluetooth_data_package_index++;
	bluetooth_sent_length = bluetooth_data_package_index;
	bluetooth_data_package_index = 0;

	if (error)
		return 1;

#ifdef BLUETOOTH_ENABLE_OK
	int16_t ok_timeout = 10000;

	while (!bluetooth_ok_received && ok_timeout>0)
	{
		//wait a millisecond
		bluetooth_delay(1);
			ok_timeout--;
		if (ok_timeout == 0)
		{
			//timeout!!
			return 2;
		}
		//check if package is available
		bluetooth_process_data();
	}
#endif

	uint16_t ms_to_timeout = timeout_ms;

	if (wait_for_response_package)
	{
		while (!bluetooth_package_received && ms_to_timeout>0)
		{
			//wait a millisecond
			bluetooth_delay(1);
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
	}


	return 0;
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


void bluetooth_address_to_array(const uint8_t *full_address, uint8_t *compressed_address, const uint8_t full_start_idx, const uint8_t compressed_start_idx, const uint8_t address_with_hyphen)
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

void bluetooth_array_to_address(const uint8_t *compressed_address, uint8_t *full_address, const uint8_t compressed_start_idx, const uint8_t full_start_idx, const uint8_t address_with_hyphen)
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



	bluetooth_cmd_sent[0] = cmd[2];
	bluetooth_cmd_sent[1] = cmd[3];

	//debug("BTM: Cmd:");
	//debug((char*)cmd);


	for (uint8_t i =0; cmd[i]!='\0'; i++)
	{

		if (bluetooth_putc(cmd[i]) == 0)
		{
			return 0;
		}

		bluetooth_delay(delay_ms);
	}

	bluetooth_delay(500);

	return 1;
}

uint8_t bluetooth_cmd_wait_response (void)
{
	while (bluetooth_response_code == 0) //Wait until bluetooth devices has responsed (handled by interrupt)
	{

		bluetooth_process_data();
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
		bluetooth_delay(2000);
		return 1;
	} else
		return 0;
}

#ifdef SQUIRREL
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
#endif

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
		bluetooth_delay(3000);
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
		bluetooth_delay(3500);
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


