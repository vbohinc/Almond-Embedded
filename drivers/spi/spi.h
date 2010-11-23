/**
 * spi.h - SPI driver for XMega
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Maximilian Karl
 */
#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h> // FIXME: Set MCU to XMega128A1
#include <common.h>

void spi_init (void);
void spi_send_byte (uint8_t byte_to_send);
uint8_t spi_receive_byte (void);
#endif
