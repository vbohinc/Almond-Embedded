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

void sd_init (void);
uint8_t sd_read_bytes (uint32_t addr, uint8_t *read_buffer, uint16_t size);
uint8_t sd_write_bytes (uint32_t addr, uint8_t *write_buffer, uint16_t size);
#endif
