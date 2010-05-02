/**
* @file bluetooth.h
* @author Stefan Profanter
* @date 26.04.2010
*
* This file contanis the definition for the functions available to
* communicate with the Bluetooth Module over USART.
* Used Bluetooth Module: BTM-222
*
*/
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#ifdef SERIAL
#include "bluetooth_serial.h"
#else
#include "./../usart/usart.h"
#endif
#include "./../../shared/fifo.h"


/**
* The size of receive buffer in bytes.
* Must be smaller than (uint8_t-2). Max value is 254.
*/
#define BLUETOOTH_DATA_PACKAGE_SIZE 30

/**
* The size of receive buffer in bytes.
* Must be smaller than (uint8_t-2). Max value is 254.
* Should be BLUETOOTH_DATA_PACKAGE_SIZE*3
*/
#define BLUETOOTH_RECEIVE_BUFFER_SIZE 90


/**
 * The special-Byte to synchronize data stream.
 */
#define BLUETOOTH_SPECIAL_BYTE 254

/**
 * The stop-Byte in combination with special byte to synchronize data stream.
 * First comes special byte, then stop byte.
 * If in data package is used the value of special byte, another special byte will be inserted.
 */
#define BLUETOOTH_STOP_BYTE 255

/**
 * Stores connection status of the bluetooth module.
 * 1 = connected (received data are data packages)
 * 0 = disconnected (received data are responses from the bluetooth module of sent commands)
 */
extern uint8_t bluetooth_is_connected;


/*! Array to put data package into. Callback is called with this array. */
extern uint8_t bluetooth_data_package[BLUETOOTH_DATA_PACKAGE_SIZE];

/**
* The number of receive buffer arrays
* Must be smaller than (uint8_t-2). Max value is 254.
*/
//#define BLUETOOTH_RECEIVE_BUFFER_ARRAYS 3


/*! Arrays to put received data in. */
//extern uint8_t bluetooth_receiveArray[BLUETOOTH_RECEIVE_BUFFER_ARRAYS][BLUETOOTH_RECEIVE_BUFFER_SIZE];


/**
* Maximum number of found bluetooth devices.
* Must be smaller or equal than 8.
*/
#define BLUETOOTH_MAX_DEVICES 8

/**
* Maximum length of the name of a bluetooth device.
* Should be 16.
*/
#define BLUETOOTH_MAX_NAME_LENGTH 16

/*! Arrays with the addresses and names of found bluetooth devices. */
extern uint8_t bluetooth_found_devices[BLUETOOTH_MAX_DEVICES][BLUETOOTH_MAX_NAME_LENGTH];

/*! Number of found bluetooth devices. */
extern uint8_t bluetooth_found_devices_count;



/**
* Initialization routine for the bluetooth module.
*/
extern void bluetooth_init(void);


extern void bluetooth_close(void);


/**
* Function will be called by the usart interrupt handling when bytes had be received and last byte was NULL.
* The bytes are stored in the global Array usart_receiveArray and the last byte is at the position usart_receiveArray_head-1.
* @param arrayNumber The number of the Array where the data is stored.
*/
extern void bluetooth_byte_received (uint8_t);



extern void bluetooth_handle_array(void);

extern uint8_t bluetooth_send_data_package(uint8_t *data, uint8_t length);


//---------------------------------------------------------
//
//               Bluetooth Commands
//
//---------------------------------------------------------

/**
 * Sends an AT-Command as a String to the bluetooth module
 * @param cmd The command as char array with '\0'. Ex: 'ATL1'
 * @return 1 on success, 0 on failure (timeout)
 */
extern uint8_t bluetooth_cmd_send (uint8_t* cmd);

/**
 * Waits until the bluetooth device returns one of the following responses. For each response will be returned a number which is given in the brackets:
 * @li OK (1)
 * @li CONNECT (2)
 * @li DISCONNECT (3)
 * @li ERROR (4)
 * @return The number of the response.
 */
extern uint8_t bluetooth_cmd_wait_response (void);

/**
 * Command: ATA
 * When it's in master mode. This command establish a connection. When it's in slave mode, the command will be rejected.
 * Connect to a Bluetooth device (It's only available when “ATD=xxxxxxxxxxxx" assigned).
 * If parameter dev_num is != 0 it connects to a Bluetooth neighborhood device 1~8 (ATF? Result).
 * @param dev_num The index of the device to connect (ATF? Result) or 0 if connect to already set Address
 * @return 1 on success, 0 otherwise
 */
extern uint8_t bluetooth_cmd_connect (const uint8_t dev_num);

/**
 * Command: ATB?
 * Get the local device BD address.
 * @return Returns the local BluetoothDevice Address as a char array. Length is normally 12 byte (char)
 */
extern uint8_t* bluetooth_cmd_get_address (void);

/**
 * Command: ATD
 * Set remote address to connecto to.
 * For security purpose, We can specifies the unique remote device can be connected. In
 * master role, it automatically inquire and search the slave even the slave is undiscoverable.
 * In slave role, the command should be as a filter condition to accept the master's inquiry.
 * If address is NULL, the remote address will be cleared.
 * @param address The remote address in the format 'xxxxxxxxxxxx' (12 Characters) or NULL to clear.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_remote_address (uint8_t* address);

/**
 * Command: ATF?
 * Search bluetooth devices.
 * The data of found devices will be stored in bluetooth_found_devices.
 * bluetooth_found_devices_count will also be set.
 * @return Returns 0 on failure otherwise the number of found devices.
 */
extern uint8_t bluetooth_cmd_search_devices (void);


/**
 * Command: ATN=xxxxx
 * Set device name.
 * Sets the friendly name of the device using 0 to 9, A to Z, a to z, space and '-'. First and last space or '-' isn't permitted.
 * @param name New name of the device. Max length is 16.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_name (uint8_t *name);

/**
 * Command: ATN?
 * Get device name.
 * Gets the friendly name of the device.
 * @param name Char array to store the name into. name must be initialized and minimum 16 bytes long.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_get_name (uint8_t *name);


/**
 * Command: ATP
 * Set device pin.
 * Sets the pin number of the device or turns the pin authorization off.
 * @param pin New pin for the device or NULL to turn it off. Max length is 4.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_pin (uint8_t *pin);

/**
 * Command: ATR
 * Set mode of the device: master or slave.
 * The device will warm-start and clear all paired addresses.
 * @param mode Master = 0, Slave = 1
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_mode (uint8_t mode);

/**
 * Command: ATZ
 * Restore factory/default settings.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_restore_settings (void);



#endif /* _BLUETOOTH_H_ */
