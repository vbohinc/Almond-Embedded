#include "../spi/spi.h"
#include "../shared/common.h"

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

#define SD_BLOCK_SIZE 512

static uint8_t sd_buffer[6];
static uint8_t sd_response_buffer[5];
static uint8_t sd_token_buffer[SD_BLOCK_SIZE+3];

void sd_init(void);
bool sd_send_command(uint8_t command_nr, uint8_t *arguments);
void sd_read_block(uint32_t addr, uint8_t *read_buffer, uint16_t size);
void sd_write_block(uint32_t addr, uint8_t *write_buffer, uint16_t size);
