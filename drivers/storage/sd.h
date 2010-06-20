#include "../drivers/spi/spi.h"
#include "../shared/common.h"

static uint8_t[6] sd_buffer;
static uint8_t[5] sd_response_buffer;
static uint8_t[35] sd_token_buffer;

void sd_init();
boolean sd_send_command(uint8_t command_nr, uint8_t *arguments);
void sd_read_block(uint8_t *addr, uint8_t *read_buffer);
void sd_write_block(uint8_t *addr, uint8_t *write_buffer);
