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
 * \author Linus Lotz
 */

#define SD_BLOCK_SIZE 512

typedef enum {
    CMD0 = 0x40,
    CMD8 = 0x48,
    CMD16 = 0x50,
    CMD17 = 0x51,
    CMD24 = 0x58,
    CMD55 = 0x77,
    CMD58 = 0x7A,
    ACMD41 = 0x69} command_t;
    
typedef enum {
    R1,
    R1B,
    R2,
    R3,
    R7} response_t;    

uint8_t sd_send_command (command_t command_nr, uint8_t *arguments);

#include <util/delay.h>
#include "sd.h"

static uint8_t sd_response_buffer[5];
static uint8_t sd_token_buffer[SD_BLOCK_SIZE + 3];

//void sd_read_block(uint8_t *addr, uint8_t *read_buffer);
//void sd_write_block(uint8_t *addr, uint8_t *write_buffer);
void sd_send_buffer (void);
void sd_get_response (uint8_t response_type);

void sd_enable (void)
{
    clear_bit (PORTD.OUT, 0);
}

void sd_disable (void)
{
    set_bit (PORTD.OUT, 0);
}

#define nop() \
    asm volatile ("nop")

void sd_init (void)
{
    debug ("Init spi ...\n");
    spi_init();
    debug ("SPI initialized\n");

    _delay_ms (100);

    sd_disable();

    unsigned char a = 0;

    for (a = 0;a < 200;a++)
    {
        nop();
    };

    debug_pgm (PSTR ("SD: warming up chip\n"));

    for (uint8_t i = 0; i < 80; ++i)
    {
        spi_receive_byte();
        spi_send_byte (0xFF);
    }

    sd_enable();

    //while(true);

    debug ("SPI initialized\n");

    /* reset card */
    uint8_t response;

    for (uint16_t i = 0;; ++i)
    {
        debug ("sending CMD0\n");
        response = sd_send_command (CMD0, NULL); //sd_raw_send_command(CMD_GO_IDLE_STATE, 0);
        byte_to_hex (response);

        if (response == (1 << R1_IDLE_STATE))
            break;

        if (i == 0x1ff)
        {
            sd_disable();
            debug_pgm (PSTR ("sd: failed (CMD0)"));
            return;
        }
    }

    debug_pgm (PSTR ("SD: SPI Init Succeeded"));
    // Place SD Card into Idle State
    sd_send_command (CMD0, NULL);
    sd_get_response (R1);
    // Chip Select Low
    //clear_bit(PORTC.OUT, 3); // Redundant if CS == SS?
    // Place SD Card into Idle State (again). Transition to SPI mode
    sd_send_command (CMD0, NULL);
    sd_get_response (R1);
    // Check supply voltage
    sd_send_command (CMD8, NULL);
    sd_get_response (R1);
    debug_pgm (PSTR ("SD: Switch to SPI Mode Succeeded. Voltage queried"));

    if (sd_response_buffer[0] == 0x04)
    { // CMD8 is illegal, Version 1 card
        do
        {
            // Prepare 'A'-Command transmission
            sd_send_command (CMD55, NULL);
            sd_get_response (R1);
            // Card initialization. Doesn't know about High Capacity cards.
            sd_send_command (ACMD41, NULL);
            sd_get_response (R1);
        }

        while (sd_response_buffer[0] != 0x00);

        debug_pgm (PSTR ("SD: Type 1 Initialization complete"));
    }

    else
    { // Version 2
        do
        {
            // Prepare 'A'-Command transmission
            sd_send_command (CMD55, NULL);
            sd_get_response (R1);
            // Card initialization. Inform it we don't like High Capacity cards.
            sd_send_command (ACMD41, NULL);
            sd_get_response (R1);
        }

        while (sd_response_buffer[0] != 0x00);

        // Gets the OCR [Operating Conditions Register] (including Card Capacity)
        sd_send_command (CMD58, NULL);

        sd_get_response (R3);

        debug_pgm (PSTR ("SD: Type 2 Initialization complete"));
    }

    // Set block size to 32 bytes
    //sd_send_command(CMD16, NULL);
    //sd_get_response(R1);
}

uint8_t sd_send_command (command_t command_nr, uint8_t* arguments)
{

    uint8_t response;
    uint8_t sd_buffer[6] = {command_nr, 0, 0, 0, 0, 0x95};

    sd_disable();
    spi_receive_byte();
    sd_enable();

    switch (command_nr)
    {

        case CMD0:
            break;

        case CMD8:
            sd_buffer[3] = 0x01;
            break;

        case CMD16:
            sd_buffer[4] = 0x20;
            break;

        case CMD17:
            // Requests a single block beginning at the address passed (4 Bytes)
            for (int i = 0; i < 4; i++)
                sd_buffer[i + 1] = arguments[i];
            break;

        case CMD24:
            for (int i = 0; i < 4; i++)
                sd_buffer[i + 1] = arguments[i]; 
            break;

        case ACMD41:
            // Static ACMD41. Initialize the card. Inform it we don't support High Capacity.
            //sd_buffer = 0x696000000001;
            sd_buffer[1] = 0x60;

            break;

        case CMD55:
            break;
            
        case CMD58:
            break;

        default:
            return 0;
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        spi_send_byte (sd_buffer[i]);
    }

    response = 0x00;

    /* receive response */

    for (uint8_t i = 0; i < 8; ++i)
    {
        response = spi_receive_byte();

        if (response != 0xff)
            break;
    }

    return response;
}

uint8_t sd_read_bytes (uint32_t addr, uint8_t *read_buffer, uint16_t size)
{
    uint32_t block_addr = addr - (addr % SD_BLOCK_SIZE);
    uint8_t bytes_read = 0;

    while (bytes_read < size)
    {
        uint8_t addr_bytes[4];

        addr_bytes[0] = block_addr >> 24;
        addr_bytes[1] = block_addr >> 16;
        addr_bytes[2] = block_addr >> 8;
        addr_bytes[3] = block_addr;

        if (sd_send_command (CMD17, addr_bytes))
        {
            debug_pgm (PSTR ("SD: CMD17 Succeeded"));
            sd_get_response (R1);

            if (sd_response_buffer[0] == 0x00)
            {
                debug_pgm (PSTR ("SD: Reading Block"));

                for (int i = 0; i < 3 + SD_BLOCK_SIZE; i++)
                {
                    sd_token_buffer[i] = spi_receive_byte();

                    if ( (i == 0) && (sd_token_buffer[i] != 0xFE))
                    { // If the first token is not a valid token bail out.
                        return 0;
                    }

                    if (i - 1 + block_addr >= addr && bytes_read < size)
                    {
                        if (read_buffer != NULL)
                            read_buffer[bytes_read++] = sd_token_buffer[i];
                    }
                }
            }
        }

        else
        {
            debug_pgm (PSTR ("SD: Read Failed"));
            return 0;
        }

        block_addr += SD_BLOCK_SIZE;
    }

    /*for (int i = 0; i < size; i++) {
     read_buffer[i] = sd_token_buffer[i+2];
     }*/
    debug_pgm (PSTR ("SD: Read Succeeded"));

    return 1;
}

uint8_t sd_write_bytes (uint32_t addr, uint8_t *write_buffer, uint16_t size)
{
    uint32_t block_addr = addr - (addr % SD_BLOCK_SIZE);
    uint8_t bytes_written = 0;

    while (bytes_written < size)
    {
        sd_read_bytes (block_addr, NULL, SD_BLOCK_SIZE);

        uint8_t addr_bytes[4];
        addr_bytes[0] = block_addr >> 24;
        addr_bytes[1] = block_addr >> 16;
        addr_bytes[2] = block_addr >> 8;
        addr_bytes[3] = block_addr;

        if (sd_send_command (CMD24, addr_bytes))
        {
            debug_pgm (PSTR ("SD: CMD24 Succeeded"));
            sd_get_response (R1);

            if (sd_response_buffer[0] == 0x00)
            {
                uint8_t start_token = 0xFE;
                spi_send_byte (start_token);

                for (int i = 0; i < 512; i++)
                {
                    if (block_addr + i < addr || block_addr + i > addr + size)
                        spi_send_byte (sd_token_buffer[i + 1]);
                    else
                    {
                        spi_send_byte (write_buffer[bytes_written++]);
                    }

                    uint8_t crc[2] =

                        { 0xFF, 0xFF }; //0xFFFF;
                    spi_send_byte (crc[0]);
                    spi_send_byte (crc[1]);
                }

                sd_token_buffer[0] = spi_receive_byte(); // Receive data response token.

                // TODO: Check status bits -> return 0 on failure
                // Busy tokens?

            }

            block_addr += SD_BLOCK_SIZE;
        }

        debug_pgm (PSTR ("SD: Write Succeeded"));
    }

    return 1;
}

void sd_get_response (uint8_t response_type)
{

    debug_pgm (PSTR ("SD: Waiting for response"));

    
    switch (response_type)
    {

        case R1:
            
            sd_response_buffer[0] = spi_receive_byte();
            debug_pgm (PSTR ("SD: R1 received:"))
            ;
            break;

        case R1b:
            sd_response_buffer[0] = spi_receive_byte();

            while ((sd_response_buffer[1] = spi_receive_byte()) == 0x00);

            break;

        case R2:
            sd_response_buffer[0] = spi_receive_byte();
            sd_response_buffer[1] = spi_receive_byte();

            break;

        case R3:

        case R7:
            for (uint8_t i = 0; i < 5; i++)
                sd_response_buffer[i] = spi_receive_byte();

            break;

        default:
            return;
    }
}

