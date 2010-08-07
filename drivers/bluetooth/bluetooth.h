/*
* @file bluetooth.h
* @author Stefan Profanter
* @date 26.04.2010
*
* This file contanis the definition for the functions available to
* communicate with the Bluetooth Module over USART.
*
* Some of the functions are only available on the Squirrel (define: SQUIRREL) because on the nut isn't enough program memory available.
*
* Used Bluetooth Module: BTM-222
*
* @see uart.c
*
*/
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <avr/io.h>
#include "./../uart/uart.h"
#include "./../../shared/fifo.h"
#include "./../../shared/error.h"


/**
 * Maximum possible size of a data package in bytes. The formula for the maximum size is (SIZE is the normal size of a package):
 * All bytes in package may be Special Byte, so before every byte must be another special byte
 * (SIZE * 2) +10;
 * Ex. real package: 14 Byte. => PACKAGE_SIZE=(2*14)+10=38
 */
#define BLUETOOTH_DATA_PACKAGE_SIZE 138

/**
* The size of receive buffer in bytes. Each received byte will be stored in this buffer
* until bluetooth_process_data function is called and data is elaborated.
* Must be smaller than (uint8_t-2). Max value is 254.
*/
#define BLUETOOTH_RECEIVE_BUFFER_SIZE 16


/**
 * The special-Byte to synchronize data stream.
 * If in data package is used the value of special byte, another special byte will be inserted.
 */
#define BLUETOOTH_SPECIAL_BYTE 255

/**
 * The stop-Byte in combination with special byte to synchronize data stream.
 * First comes special byte, then stop byte.
 */
#define BLUETOOTH_STOP_BYTE 254

/**
 * The resent-Byte in combination with special byte to tell connected client to resent received data package due to timeout.
 * First comes special byte, then resent byte.
 */
#define BLUETOOTH_RESENT_BYTE 253

/*
 * Is 1 if bluetooth is currently connected, 0 otherwise.
 */
extern uint8_t bluetooth_is_connected;

/**
* Initialization routine for the bluetooth module.
* @param bluetooth_callback_handler Callback handler for received data.
*
* @li data_package contains the received data or the Connect-Address or disconnect-Address
* @li callback_type 0 for data package, 1 for connect, 2 for disconnect
* @li data_length Length of the received data or the address
*/
extern void bluetooth_init(void (*bluetooth_callback_handler)(char *data_package, const uint8_t callback_type, const uint8_t data_length));


/**
 * Callback handler for received bytes or connect/disconnet notification
 * @li data_package contains the received data or the Connect-Address or disconnect-Address
 * @li callback_type 0 for data package, 1 for connect, 2 for disconnect
 * @li data_length Length of the received data or the address
 */
void (*bluetooth_callback)(char *data_package, const uint8_t callback_type, const uint8_t data_length);

/**
 * Writes the byte to the usart connection
 * @return 1 for success, 0 if error.
 */
extern int bluetooth_putc(const uint8_t byte);

/**
 * Called by bluetooth_process_data if a stop byte was received or the data-package is complete.
 */
void bluetooth_handle_array(void);

/**
 * Sends the data to the connected client.
 * Adds automatically the stop-byte combination at the end of the package
 * @param data The data to send.
 * @param length the length (number of bytes) in the data array to send.
 * @return 0 on success, 1 on error
 */
extern uint8_t bluetooth_send_data_package(uint8_t *data, const uint8_t length);

/**
 * Sends the data to the connected client and waits for a response package.
 * Adds automatically the stop-byte combination at the end of the package
 * @param data The data to send. After response is received, it contains the response package. Must be big enough for the response package.
 * @param length the length (number of bytes) in the data array to send. After response is received, contains the length of the received package.
 * @param timeout_ms timeout in ms to wait for response
 * @return 0 on success, 1 on error, 2 on timeout
 */
extern uint8_t bluetooth_send_data_package_with_response(uint8_t *data, uint8_t *length, const uint16_t timeout_ms);

/**
 * Processes received data stored in the FIFO. Should be called in the while-loop of the main program to handle receive buffer of uart.
 * If the uart buffer is full, new received bytes will be dropped until this function is called and the buffer will be read/cleared.
 */
extern void bluetooth_process_data(void);

/**
 * Switches the Bluetooth-Module to Master mode (Module connects to oder device).
 * Before setting to master, checks if it is already master. If yes, then it doesn't need to switch again to master.
 * If it isn't master, it switches to master mode and disables autoconnect.
 * @return 1 on success, 0 on failure
 */
extern uint8_t bluetooth_set_as_master(void);

/**
 * Switches the Bluetooth-Module to Slave mode (Module waits for connections).
 * Before setting to slave, checks if it is already slave. If yes, then it doesn't need to switch again to slave.
 * If it isn't slave, it switches to slave mode and enables autoconnect.
 * @return 1 on success, 0 on failure
 */
extern uint8_t bluetooth_set_as_slave(void);

/**
 * Test if connection with bluetooth module is OK.
 * @param tries Number of tries
 * @return 1 on successful test, 0 otherwise
 */
extern uint8_t bluetooth_test_connection(uint8_t tries);


/**
 * Converts the string representation of a bluetooth address into its compressed format
 * where each byte contains 2 chars (hex values).
 * @param full_address the full address in the format xxxxxxxxxxxx or xxxx-xx-xxxxxx
 * @param compressed_address allocated array with minimum 6 bytes to store compressed address
 * @param address_with_hyphen 1 if address is in the format xxxx-xx-xxxxxx or 0 if address is without hyphen xxxxxxxxxxxx
 */
extern void bluetooth_address_to_array(const char *full_address, char *compressed_address, const uint8_t address_with_hyphen);

/**
 * Converts the compressed address format into the string representation of a bluetooth address.
 * @param compressed_address Array with 6 bytes which contain the compressed address
 * @param full_address allocated array with minimum 12 bytes to store full address in the format xxxxxxxxxxxx or with hyphen
 * @param address_with_hyphen 1 if address should be in the format xxxx-xx-xxxxxx or 0 if address should be without hypen xxxxxxxxxxxx
 */
extern void bluetooth_array_to_address(const char *compressed_address, char *full_address, const uint8_t address_with_hyphen);

//---------------------------------------------------------
//
//               Bluetooth Commands
//
//---------------------------------------------------------

/**
 * Sends an AT-Command as a String to the bluetooth module.
 * @param cmd The command as char array with nullbyte. Ex: 'ATL1'
 * @return 1 on success, 0 on failure (timeout)
 */
uint8_t bluetooth_cmd_send (const char* cmd);

/**
 * Command: AT
 * Check connection to bluetooth device
 * @return Returns 1 on success check, 0 otherwise if timeout occured or error returned
 *
 */
extern uint8_t bluetooth_cmd_test_connection (void);

#ifdef SQUIRREL

/**
 * Command: ATA
 * When it's in master mode. This command establish a connection. When it's in slave mode, the command will be rejected.
 * Connect to a Bluetooth device (It's only available when “ATD=xxxxxxxxxxxx" assigned).
 * If parameter dev_num is != 0 it connects to a Bluetooth neighborhood device 1~8 (ATF? Result).
 * @param dev_num The index of the device to connect (ATF? Result) or 0 if connect to already set Address
 * @return 1 on success, 0 otherwise
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_connect (const uint8_t dev_num);

/**
 * Command: ATB?
 * Get the local device BD address.
 * @return Returns the local BluetoothDevice Address as a char array (in one byte are 2 char-values). Length is normally 6 byte (char)
 *
 * Only on SQUIRREL.
 */
extern char* bluetooth_cmd_get_address (void);

#endif

/**
 * Command: ATD
 * Set remote address to connect to.
 * For security purpose, We can specify the unique remote device to connect. In
 * master role, it automatically inquire and search the slave even the slave is undiscoverable.
 * In slave role, the command should be as a filter condition to accept the master's inquiry.
 * If address is NULL, the remote address will be cleared.
 * @param address The remote address with length 6 (each byte contains 2 char) in the format 'xx|xx|xx|xx|xx|xx'
 * 				(12 Characters in 6 bytes) or NULL to clear. USe the function bluetooth_address_to_array to convert
 * 				from xxxxxxxxxxxx to xx|xx|xx|xx|xx|xx.
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_remote_address (const char* address);

#ifdef SQUIRREL

/**
 * Command: ATF?
 * Search bluetooth devices.
 * The data of found devices will be stored in bluetooth_found_devices.
 * bluetooth_found_devices_count will also be set.
 * @return Returns NULL on failure otherwise a pointer to bluetooth_data_package with found devices.
 *
 * @see bluetooth_data_package
 *
 * Only on SQUIRREL.
 */
extern char* bluetooth_cmd_search_devices (void);

/**
 * Command: ATH (only in online command mode)
 * Drop (close) connection.
 * @return Returns 1 on success otherwise 0.
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_close_connection (void);

/**
 * Command: ATH1 / ATH0
 * Set discoverable mode.
 * @param discoverable 1 for yes, 0 for no
 * @return Returns 1 on success otherwise 0.
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_discoverable (const uint8_t discoverable);


/**
 * Command: ATN=xxxxx
 * Set device name.
 * Sets the friendly name of the device using 0 to 9, A to Z, a to z, space and '-'. First and last space or '-' isn't permitted.
 * @param name New name of the device. Max length is 16.
 * @return Returns 1 on success otherwise 0.
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_set_name (const char *name);

/**
 * Command: ATN?
 * Get device name.
 * Gets the friendly name of the device. Maximum 16 bytes long.
 * @return Returns a pointer to bluetooth_data_package with name on success, otherwise NULL.
 *
 * @see bluetooth_data_package
 *
 * Only on SQUIRREL.
 */
extern char* bluetooth_cmd_get_name (void);

#endif


/**
 * Command: ATO
 * Enable/Disable autoconnect.
 * @param autoconnect 1 for autoconnect with address of ATD or 0 to disable
 * @return 1 on success or 0.
 */
extern uint8_t bluetooth_cmd_autoconnect (const uint8_t autoconnect);

#ifdef SQUIRREL

/**
 * Command: ATP
 * Set device pin.
 * Sets the pin number of the device or turns the pin authorization off.
 * @param pin New pin for the device or NULL to turn it off. Max length is 4.
 * @return Returns 1 on success otherwise 0.
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_set_pin (const char *pin);

/**
 * Command: ATP?
 * Get device pin.
 * Gets the connection pin of the device
 * @return Returns array with pin on success otherwise NULL.
 *
 * Only on SQUIRREL.
 */
extern char* bluetooth_cmd_get_pin (void);

#endif


/**
 * Command: ATR
 * Set mode of the device: master or slave.
 * The device will warm-start and clear all paired addresses.
 * @param mode Master = 0, Slave = 1
 * @return Returns 1 on success otherwise 0.
 */
extern uint8_t bluetooth_cmd_set_mode (uint8_t mode);

/**
 * Command: ATR?
 * Get mode of the device: master(0) or slave(1).
 * @return Returns array with mode at index 0 or NULL if error.
 */
extern char *bluetooth_cmd_get_mode (void);

#ifdef SQUIRREL
/**
 * Command: ATZ
 * Restore factory/default settings.
 * @return Returns 1 on success otherwise 0.
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_restore_settings (void);

/**
 * Command: +++
 * Switch from Data to Online-Command mode.
 * Use bluetooth_cmd_close_connection to close a connection from a connected device.
 * @return Returns 1 on success otherwise 0.
 *
 * Only on SQUIRREL.
 */
extern uint8_t bluetooth_cmd_online_command (void);
#endif


#endif /* _BLUETOOTH_H_ */
