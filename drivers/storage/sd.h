/**
 * Driver for a SD Card
 * @file sd.h
 * @author Stefan Profanter
 * @author Sean Labastille
 */
#ifndef __SD_H__
#define __SD_H__
#include <spi/spi.h>
#include <common.h>
#include <drivers/display/display.h>

#define CMD0 0
#define CMD8 1
#define CMD16 2
#define CMD17 3
#define CMD24 7
#define CMD55 4
#define ACMD41 5
#define CMD58 6
#define R1 1
#define R1b 2
#define R2 3
#define R3 4
#define R7 5

///SD block size define
#define SD_BLOCK_SIZE 512

/// Initialize SD card
void sd_init (void);

/**
* \brief send sd command
* sends an commad with arguments to the sd card
* @param command_nr is the number of the wanted command
* @param arguments pointer array with all arguments to be passed
* @return the response code of the sd card
*/
uint8_t sd_send_command (uint8_t command_nr, uint8_t *arguments);

/**
* \brief read bytes from sd
* reads bytes on sd card at addr into buffer
* @param addr address on sd card to where start reading
* @param read_buffer pointer where the read data will be saved to
* @param size of how much will be read
*/
uint8_t sd_read_bytes (uint32_t addr, uint8_t *read_buffer, uint16_t size);

/**
* \brief write bytes to sd
* writes bytes from a buffer to th sd card
* @param addr on sd card where to start rading
* @param write_buffer pointer to the data to write to sd
* @param size of the data to write 
*/
uint8_t sd_write_bytes (uint32_t addr, uint8_t *write_buffer, uint16_t size);
#endif
