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

uint8_t bluetooth_data_package[BLUETOOTH_DATA_PACKAGE_SIZE];

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


/*! Count of unhandled arrays. If no one is currently unhandled, the value is set to 0*/
//uint8_t bluetooth_receiveArray_tohandle_count = 0;

uint8_t bluetooth_found_devices[BLUETOOTH_MAX_DEVICES][BLUETOOTH_MAX_NAME_LENGTH];

/*! Number of found bluetooth devices. */
uint8_t bluetooth_found_devices_count=0;

/*! Buffer to build cmd to send or to parse resonse of bluetooth device */
uint8_t bluetooth_cmd_buffer[20];

/*! When a response for a command comes in, this variable will be set to the value of the response.
 * @see bluetooth_cmd_wait_response
 */
uint8_t bluetooth_response_code = 0;


void bluetooth_init()
{
	fifo_init (&bluetooth_infifo,   bluetooth_inbuffer, BLUETOOTH_RECEIVE_BUFFER_SIZE);
	usart_set_byte_handler(bluetooth_byte_received);
	//Make all receiveBuffer arrays available to store data.
	/*for (uint8_t i=0; i<BLUETOOTH_RECEIVE_BUFFER_ARRAYS; i++)
	{
		bluetooth_receiveArray_handled[i] = 1;
	}*/
}

void inline bluetooth_byte_received (uint8_t byte)
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

void inline bluetooth_process_data (void)
{
	while (bluetooth_infifo.count>0)
	{ //read all bytes from fifo

		if (bluetooth_is_connected==0)
		{
			//Data in FIFO is response to a sent command

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

void bluetooth_handle_array(void)
{
	//Fill empty indexes with 0 (there shouldn't be any!!)
	for (uint8_t i=bluetooth_data_package_index; i<BLUETOOTH_DATA_PACKAGE_SIZE;i++)
		bluetooth_data_package[i]=0;

	//Call callback function
}

uint8_t bluetooth_send_data_package(uint8_t *data, uint8_t length)
{
	uint8_t error = 0;
	for (uint8_t i=0; i<length && error==0; i++)
	{
		if (data[i]==BLUETOOTH_SPECIAL_BYTE)
			error = (error || (usart_putc(BLUETOOTH_SPECIAL_BYTE,  F_CPU)==0)); //F_CPU cycles is approx. 1 second
		error = (error || (usart_putc(data[i],  F_CPU)==0)); //F_CPU cycles is approx. 1 second
	}
	//send stop byte
	error = (error || (usart_putc(BLUETOOTH_SPECIAL_BYTE,  F_CPU)==0)); //F_CPU cycles is approx. 1 second
	error = (error || (usart_putc(BLUETOOTH_STOP_BYTE,  F_CPU)==0)); //F_CPU cycles is approx. 1 second
	return (error==0);
}



//---------------------------------------------------------
//
//               Bluetooth Commands
//
//---------------------------------------------------------


uint8_t bluetooth_cmd_send (uint8_t* cmd)
{
	bluetooth_response_code = 0;
	uint8_t command_length = 0;
	while (cmd[command_length]!=0)
		command_length++;
	usart_send_bytes(cmd, command_length-1, F_CPU); //F_CPU cycles is approx. 1 second
	return 0;
}

uint8_t bluetooth_cmd_wait_response (void)
{
	while (bluetooth_response_code == 0) //Wait until bluetooth devices has resonsed (handled by interrupt)
		;
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
	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==2); //CONNECT
}

uint8_t* bluetooth_cmd_get_address (void)
{
	bluetooth_cmd_buffer[0] = 'A';
	bluetooth_cmd_buffer[1] = 'T';
	bluetooth_cmd_buffer[2] = 'B';
	bluetooth_cmd_buffer[2] = '?';
	bluetooth_cmd_buffer[4] = 13; //<CR>
	bluetooth_cmd_buffer[5] = 0;

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	if (bluetooth_cmd_wait_response()!=1) //OK
		return NULL;
	else
		return bluetooth_cmd_buffer;
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

	if (bluetooth_cmd_send(bluetooth_cmd_buffer) == 0)
		return 0;

	return (bluetooth_cmd_wait_response()==1); //OK
}

/**
 * Command: ATF?
 * Search bluetooth devices.
 * The data of found devices will be stored in bluetooth_found_devices.
 * bluetooth_found_devices_count will also be set.
 * @return Returns 0 on failure otherwise the number of found devices.
 */
uint8_t bluetooth_cmd_search_devices (void)
{
	return 0;
}


/**
 * Command: ATN=xxxxx
 * Set device name.
 * Sets the friendly name of the device using 0 to 9, A to Z, a to z, space and '-'. First and last space or '-' isn't permitted.
 * @param name New name of the device. Max length is 16.
 * @return Returns 1 on success otherwise 0.
 */
uint8_t bluetooth_cmd_set_name (uint8_t *name)
{
	return 0;

}

/**
 * Command: ATN?
 * Get device name.
 * Gets the friendly name of the device.
 * @param name Char array to store the name into. name must be initialized and minimum 16 bytes long.
 * @return Returns 1 on success otherwise 0.
 */
uint8_t bluetooth_cmd_get_name (uint8_t *name)
{
	return 0;

}


/**
 * Command: ATP
 * Set device pin.
 * Sets the pin number of the device or turns the pin authorization off.
 * @param pin New pin for the device or NULL to turn it off. Max length is 4.
 * @return Returns 1 on success otherwise 0.
 */
uint8_t bluetooth_cmd_set_pin (uint8_t *pin)
{
	return 0;

}

/**
 * Command: ATR
 * Set mode of the device: master or slave.
 * The device will warm-start and clear all paired addresses.
 * @param mode Master = 0, Slave = 1
 * @return Returns 1 on success otherwise 0.
 */
uint8_t bluetooth_cmd_set_mode (uint8_t mode)
{
	return 0;

}

/**
 * Command: ATZ
 * Restore factory/default settings.
 * @return Returns 1 on success otherwise 0.
 */
uint8_t bluetooth_cmd_restore_settings (void)
{
	return 0;

}


