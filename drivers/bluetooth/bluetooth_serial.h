/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
                                  ___           ___                       ___           ___                       ___     
     _____                       /__/\         /  /\          ___        /  /\         /  /\          ___        /__/\    
    /  /::\                      \  \:\       /  /:/_        /  /\      /  /::\       /  /::\        /  /\       \  \:\   
   /  /:/\:\    ___     ___       \  \:\     /  /:/ /\      /  /:/     /  /:/\:\     /  /:/\:\      /  /:/        \__\:\  
  /  /:/~/::\  /__/\   /  /\  ___  \  \:\   /  /:/ /:/_    /  /:/     /  /:/  \:\   /  /:/  \:\    /  /:/     ___ /  /::\ 
 /__/:/ /:/\:| \  \:\ /  /:/ /__/\  \__\:\ /__/:/ /:/ /\  /  /::\    /__/:/ \__\:\ /__/:/ \__\:\  /  /::\    /__/\  /:/\:\
 \  \:\/:/~/:/  \  \:\  /:/  \  \:\ /  /:/ \  \:\/:/ /:/ /__/:/\:\   \  \:\ /  /:/ \  \:\ /  /:/ /__/:/\:\   \  \:\/:/__\/
  \  \::/ /:/    \  \:\/:/    \  \:\  /:/   \  \::/ /:/  \__\/  \:\   \  \:\  /:/   \  \:\  /:/  \__\/  \:\   \  \::/     
   \  \:\/:/      \  \::/      \  \:\/:/     \  \:\/:/        \  \:\   \  \:\/:/     \  \:\/:/        \  \:\   \  \:\     
    \  \::/        \__\/        \  \::/       \  \::/          \__\/    \  \::/       \  \::/          \__\/    \  \:\    
     \__\/                       \__\/         \__\/                     \__\/         \__\/                     \__\/    

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

#include "stdint.h"

#endif

/*! Callback handler for received bytes */
void (*bluetooth_serial_byte_handling_function)(uint8_t);

/**
* Initializes the serial port and starts the read thread.
* Call only once on program startup.
*/
extern void bluetooth_serial_init(void);

/**
 * Close serial port and stop all threads.
 */
extern void bluetooth_serial_close(void);

/**
* This function sets the callback function for a received byte.
* @param byte_handling_function callback function to call when when a byte was received
*/
extern void bluetooth_serial_set_byte_handler(void (*byte_handling_function)(uint8_t));

/**
 * Sends the given bytes over the serial port.
* @param bytes The byte array to send
* @param length The length of the byte array
* @return 1 on success or 0 on timeout
 */
extern uint8_t bluetooth_serial_send_bytes(uint8_t *bytes, const uint8_t length);

/**
* Send a byte over the USART module.
* @param byte to send
* @return 1 on success or 0 on timeout
*/
extern uint8_t bluetooth_serial_putc (const uint8_t byte);

/**
* Function will be called when a byte was read.
* @param byte the received byte
*/
extern void bluetooth_serial_byte_received(const uint8_t byte);

#endif /* _BLUETOOTH_SERIAL_H_ */
