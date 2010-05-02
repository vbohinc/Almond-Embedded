/**
* @file usart.h
* @author Stefan Profanter
* @date 26.04.2010
*
* This file contanis the definition for the functions available to
* communicate with the USART Module and configure it on the AVR microcontroller.
* Due to different used microcontrollers and therefore different USART registers
* two defines are used to distinguish between the two microcontrollers.
* ATmega8535   = ATMEGA8535
* ATxmega128A1 = ATXMEGA
*
*/
#ifndef _USART_H_
#define _USART_H_

#include <avr/io.h>
#include <avr/interrupt.h>

 #if !defined(null)
 #define NULL 0
 #endif

/*! Callback handler for received bytes */
void (*usart_byte_handling_function)(uint8_t);

/**
* The baudrate for USART <-> Bluetooth
*/
#define USART_BAUD        19200UL

/**
* The value for USART_BAUD to store in UBRR register
*/
#define USART_UBRR_BAUD   ((F_CPU/(16UL*USART_BAUD))-1)

/**
* Initializes the USART module on the microcontroller.
* Call only once on program startup.
*/
extern void usart_init(void);

/**
* This function sets the callback function for a received byte.
* @param byte_handling_function callback function to call when when a byte was received
*/
extern void usart_set_byte_handler(void (*byte_handling_function)(uint8_t));



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
extern uint8_t usart_send_bytes(uint8_t *bytes, const uint8_t length , const uint32_t cycle_count_timeout);

/**
* Send a byte over the USART module.
* If cycle_count_timeout is 0 waits until the send-buffer is empty and send then the byte. If cycle_count_timeout > 0
* waits until the send-buffer is empty and send then the byte or returns 1 on timeout.
* cycle_count_timeout defines the approx count of cycles for timeout.
* @param byte to send
* @param cycle_count_timeout send timeout or 0 for no timeout
* @return 1 on success or 0 on timeout
*/
extern uint8_t usart_putc (const uint8_t byte, const uint32_t cycle_count_timeout);

/**
* Function will be called by the RXComplete interrupt when a new byte is in the usart buffer.
* The new byte will be added to the usart_receiveArray at indec usart_receiveArray_current at the position usart_receiveArray_head.
* If array is full, usart_receiveArray_handled[usart_receiveArray_current] will be set to 0, usart_bytes_handling_function(usart_receiveArray_current)
* will be called and afterwards usart_receiveArray_handled[usart_receiveArray_current] will be reset to 1.
* @param byte the received byte
*/
extern void usart_byte_received(const uint8_t byte);


#endif /* _USART_H_ */
