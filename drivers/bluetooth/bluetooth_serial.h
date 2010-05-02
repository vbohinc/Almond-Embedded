/**
* @file bluetooth_serial.h
* @author Stefan Profanter
* @date 02.05.2010
*
* This file contains functions for test purpose on the pc. It replaces the function of the USART on the
* microcontroller and is used, if the BTM-222 is connected over USB to the PC
*
*/

#ifndef _BLUETOOTH_SERIAL_H_
#define _BLUETOOTH_SERIAL_H_


#include <termios.h>

#ifdef SERIAL

#include "./../../shared/BaseTypesBackend.h"

#endif

/*! Callback handler for received bytes */
void (*bluetooth_serial_byte_handling_function)(uint8_t);

/**
* Initializes the USART module on the microcontroller.
* Call only once on program startup.
*/
extern void bluetooth_serial_init(void);

extern void bluetooth_serial_close(void);

/**
* This function sets the callback function for a received byte.
* @param byte_handling_function callback function to call when when a byte was received
*/
extern void bluetooth_serial_set_byte_handler(void (*byte_handling_function)(uint8_t));

/**
 * Sends the given bytes over usart.
* If cycle_count_timeout is 0 waits until the send-buffer of the usart module is empty and send then one byte. If cycle_count_timeout > 0
* waits until the send-buffer is empty and send then one byte or returns 1 on timeout.
* cycle_count_timeout defines the approx count of cycles for timeout.
* @param bytes The byte array to send
* @param length The length of the byte array
* @param cycle_count_timeout send timeout or 0 for no timeout
* @return 1 on success or 0 on timeout
 */
extern uint8_t bluetooth_serial_send_bytes(uint8_t *bytes, const uint8_t length);

/**
* Send a byte over the USART module.
* If cycle_count_timeout is 0 waits until the send-buffer is empty and send then the byte. If cycle_count_timeout > 0
* waits until the send-buffer is empty and send then the byte or returns 1 on timeout.
* cycle_count_timeout defines the approx count of cycles for timeout.
* @param byte to send
* @param cycle_count_timeout send timeout or 0 for no timeout
* @return 1 on success or 0 on timeout
*/
extern uint8_t bluetooth_serial_putc (const uint8_t byte);

/**
* Function will be called by the RXComplete interrupt when a new byte is in the usart buffer.
* The new byte will be added to the usart_receiveArray at indec usart_receiveArray_current at the position usart_receiveArray_head.
* If array is full, usart_receiveArray_handled[usart_receiveArray_current] will be set to 0, usart_bytes_handling_function(usart_receiveArray_current)
* will be called and afterwards usart_receiveArray_handled[usart_receiveArray_current] will be reset to 1.
* @param byte the received byte
*/
extern void bluetooth_serial_byte_received(const uint8_t byte);

#endif /* _BLUETOOTH_SERIAL_H_ */
