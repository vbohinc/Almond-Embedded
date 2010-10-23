#ifndef __SD_H__
#define __SD_H__
#include "../spi/spi.h"
#include "../shared/common.h"

#define CMD0 0
#define CMD8 8
#define CMD16 16
#define CMD17 17
#define CMD24 24
#define CMD55 55
#define ACMD41 41
#define CMD58 58
#define R1 1
#define R1b 2
#define R2 3
#define R3 4
#define R7 5

#define SD_BLOCK_SIZE 512

void sd_init(void);
uint8_t sd_send_command(uint8_t command_nr, uint8_t *arguments);
uint8_t sd_read_bytes(uint32_t addr, uint8_t *read_buffer, uint16_t size);
uint8_t sd_write_bytes(uint32_t addr, uint8_t *write_buffer, uint16_t size);
#endif
