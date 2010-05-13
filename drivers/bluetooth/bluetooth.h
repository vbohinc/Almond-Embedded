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

/*! Array to put data package to send into. Also Callback is called with this array to return a received package.
 * This array is also used to return a response of a command.
 * The biggest response is of the ATF? command.
 * The maximum number of found devices is: 8
 * The maximum lenght of a name is: 16
 * The length of an address is: 12 (without '-')
 * The data is stored in the following format (where x is the index of found device: Device '1' is at index '0'):
 * arr[0]: Number of found devices
 * arr[1+x*(16+12)] to arr[1+x*(16+12)+15]: Name of found device.
 * 		Ex: arr[1] to arr[15] contains first name terminated by null if shorter than 16 chars
 * arr[1+x*(16+12)+16] to arr[1+x*(16+12)+25]: address of found device.
 * In total there are 1+(16+12)*8=225 bytes needed.
 * ATTENTION: change also in .c file the size!!
 */
extern uint8_t bluetooth_data_package[225];

/*
 * Contains the address of the connected device or [0]=0 if disconnected
 */
extern uint8_t bluetooth_is_connected;

/**
* Initialization routine for the bluetooth module.
*/
extern void bluetooth_init(void (*bluetooth_callback_handler)(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length));


extern void bluetooth_close(void);


/**
* Function will be called by the usart interrupt handling when bytes had be received and last byte was NULL.
* The bytes are stored in the global Array usart_receiveArray and the last byte is at the position usart_receiveArray_head-1.
* @param arrayNumber The number of the Array where the data is stored.
*/
extern void bluetooth_byte_received (uint8_t);


void (*bluetooth_callback)(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length);

extern void bluetooth_handle_array(void);

extern uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length);


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
extern uint8_t bluetooth_cmd_send (const uint8_t* cmd, const uint16_t delay_ms);

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
 * Command: AT
 * Check connection to bluetooth device
 * @return Returns 1 on success check, 0 otherwise if timeout occured or error returned
 * @todo add timeout
 */
extern uint8_t bluetooth_cmd_test_connection (void);

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
extern uint8_t bluetooth_cmd_set_remote_address (const uint8_t* address);

/**
 * Command: ATF?
 * Search bluetooth devices.
 * The data of found devices will be stored in bluetooth_found_devices.
 * bluetooth_found_devices_count will also be set.
 * @return Returns NULL on failure otherwise the array with found devices.
 * @todo add format description
 */
extern uint8_t* bluetooth_cmd_search_devices (void);

/**
 * Command: ATH (only in online command mode)
 * Drop (close) connection.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_close_connection (void);

/**
 * Command: ATH1 / ATH0
 * Set discoverable mode.
 * @param discoverable 1 for yes, 0 for no
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_discoverable (const uint8_t discoverable);


/**
 * Command: ATN=xxxxx
 * Set device name.
 * Sets the friendly name of the device using 0 to 9, A to Z, a to z, space and '-'. First and last space or '-' isn't permitted.
 * @param name New name of the device. Max length is 16.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_name (const uint8_t *name);

/**
 * Command: ATN?
 * Get device name.
 * Gets the friendly name of the device.
 * @param name Char array to store the name into. name must be initialized and minimum 16 bytes long.
 * @return Returns array with name on success otherwise NULL.
 */
extern uint8_t* bluetooth_cmd_get_name (const uint8_t *name);


/**
 * Command: ATO
 * Enable/Disable autoconnect.
 * @param autoconnect 1 for autoconnect with address of ATD or 0 to disable
 * @return 1 on success or 0.
 */
extern uint8_t bluetooth_cmd_autoconnect (const uint8_t autoconnect);


/**
 * Command: ATP
 * Set device pin.
 * Sets the pin number of the device or turns the pin authorization off.
 * @param pin New pin for the device or NULL to turn it off. Max length is 4.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_pin (const uint8_t *pin);

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

/**
 * Command: +++
 * Switch from Data to Online-Command mode
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_online_command (void);



#endif /* _BLUETOOTH_H_ */
