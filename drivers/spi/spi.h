#include <avr/io.h> // FIXME: Set MCU to XMega128A1
#include "../shared/common.h"

void spi_init(void);
void spi_send_byte(uint8_t byte_to_send);
uint8_t spi_receive_byte(void);
