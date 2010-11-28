/**
 * sd.c - Driver for a SD Card
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Stefan Profanter
 * \author Sean Labastille
 */
#ifndef __SD_H__
#define __SD_H__
#include <spi/spi.h>
#include <common.h>
#include <drivers/display/display.h>

#define CMD0 0
#define CMD1 100
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

#define SD_BLOCK_SIZE 512

int sd_init (void);
void sd_send_command (uint8_t command_nr, uint8_t *arguments);
uint8_t sd_read_bytes (uint32_t addr, uint8_t *read_buffer, uint16_t size);
uint8_t sd_write_bytes (uint32_t addr, uint8_t *write_buffer, uint16_t size);
#endif
