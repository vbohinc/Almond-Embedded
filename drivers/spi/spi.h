/**
 * SPI driver for XMega
 * @file spi.h
 * @author Maximilian Karl
 */
#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h> // FIXME: Set MCU to XMega128A1
#include <common.h>

///Initialize SPI
void spi_init (void);
/**
* Sends byte over SPI
* @param byte_to_send holds the byte to be send
*/
void spi_send_byte (uint8_t byte_to_send);
/**
* Recieve byte over SPI
* @return uint8_t with recieved byte
*/
uint8_t spi_receive_byte (void);
#endif
