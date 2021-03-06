/**
 * Driver for twi on XMega and ATMega
 * @file twi.h
 * @author Linus Lotz
 */

/**
 @brief a simple api for the twi aka I2C
 Simple API for TWI aka I2C, only master transmitter/reciever mode supported for now
 */

#ifndef TWI_H_INCLUDED
#define TWI_H_INCLUDED

#include <avr/io.h>
#include <stdbool.h>

/**
 Set the frequency of the twi bus
 */
void twi_init (void);

/**
 The access mode when connecting, either READ or WRITE
 */
enum twi_access_mode { READ = 1, WRITE = 0};

/**
 enum if sending of an ack is wished
 */
enum twi_send_ack { ACK = 1, NACK = 0};

/**
 Send RESTART on the twi bus
 */
#if __AVR_ARCH__ <= 6
uint8_t twi_start(void);
#else
uint8_t twi_restart (void);
#endif

/**
 Send STOP on the twi bus
 */
void twi_stop (void);

/**
 connect to specific slave will send start and address
 @param mode access mode to use
 @param address address of the slave (a 7 bit number)
 @return either 0 for success or 1 when faild (don't now if this actually happens)
 */
uint8_t twi_connect (enum twi_access_mode mode, uint8_t address);

/**
 send a byte to the slave
 @param data the data to send
 @return either 0 for success or 1 on failure (don't now if this actually happens)
 */
uint8_t twi_write (uint8_t data);

/**
 get a byte from a slave
 @param data the data that was recieved
 @param ack ACK to send an ack after recieving or NACK to send NACK afterwards
 @return either 0 for success or 1 on failure (don't now if this actually happens)
 */
uint8_t twi_read (uint8_t * data, enum twi_send_ack ack);

/**
 get the raw status code
 @return the status code
 */
uint8_t twi_status (void);

/** \brief accept to commit twi ackoledge
* only needed on xmega, where you have to either stop restart or accept after a read(to commit the acknoledge)
*/
void twi_accept (void);

#endif // TWI_H_INCLUDED

