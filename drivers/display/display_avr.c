
/**
 * display_avr.c - the display driver avr version
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Thomas Parsch
 */
#include <avr/io.h>
#include <util/delay.h>

// LINES
#define DISPLAY_PAGE_NUMBER 7   // Starts with Page 0, here: 8 pages overall
#define DISPLAY_PAGE_INIT 0

// COLUMNS
#define DISPLAY_COL_NUMBER 127 // Starts with column 0, here: 132 columns overall
#define DISPLAY_COL_HEIGHT 8
#define DISPLAY_COL_INIT 0 //normal: 0, reverse: 4
#define DISPLAY_COL_OFFSET 1

// DISPLAY PORTS
#define DISPLAY_CS  6       // Chip select
#define DISPLAY_RST 7       // Reset
#define DISPLAY_RS  5       // Register Select input
#define DISPLAY_RD  3       // Read
#define DISPLAY_WR  4       // Write

#define DISPLAY_BACKBUFFER_COLUMNS 128
#define DISPLAY_BACKBUFFER_LINES 8

static uint8_t
backbuffer[DISPLAY_BACKBUFFER_LINES][DISPLAY_BACKBUFFER_COLUMNS];

// Display command type
enum
{
    DISPLAY_CMD,
    DISPLAY_DATA
};

inline static void display_send (uint8_t value, uint8_t mode);

void
display_init (void)
{
    //Background light
    set_bit (PORTH.DIR, 2);
    set_bit (PORTH.OUT, 2);

    //User system setup by external pins
    PORTF.DIR = 0xFF;
    set_bit (PORTH.DIR, DISPLAY_RS);
    set_bit (PORTH.DIR, DISPLAY_RST);
    set_bit (PORTH.DIR, DISPLAY_CS);
    set_bit (PORTH.DIR, DISPLAY_WR);
    set_bit (PORTH.DIR, DISPLAY_RD);
    set_bit (PORTH.OUT, DISPLAY_CS);
    clear_bit (PORTH.OUT, DISPLAY_RST);
    set_bit (PORTH.OUT, DISPLAY_RS);
    set_bit (PORTH.OUT, DISPLAY_WR);
    set_bit (PORTH.OUT, DISPLAY_RD);

    _delay_ms (1000);       //Waiting for stabilizing power

    set_bit (PORTH.OUT, DISPLAY_RST);

    display_send (0xA2, DISPLAY_CMD);   //LCD Bias Select
    //display_send(0xA1, DISPLAY_CMD);      //ADC SELECT
    //display_send(0xC8, DISPLAY_CMD);      //SHL Direction 0-64

    display_send (0x25, DISPLAY_CMD);   //Regulator resistor select
    display_send (0x81, DISPLAY_CMD);   //Set reference voltage mode        contrast
    display_send (0x30, DISPLAY_CMD);   //Set reference voltage register    contrast value
    display_send (0x00, DISPLAY_CMD);
    display_send (0x2F, DISPLAY_CMD);   //PowerControl
    display_send (0x40, DISPLAY_CMD);   //Initial Display line to 0
    display_send (0xA6, DISPLAY_CMD);   //REVERSE DISPLAY OFF
    display_send (0xAF, DISPLAY_CMD);   // Display ON

    display_send (0xA0, DISPLAY_CMD);   //ADC SELECT
    display_send (0xC0, DISPLAY_CMD);   //SHL Select

    display_send (0xB0, DISPLAY_CMD);   // SET PAGE ADDRESS 0
    display_send (0x10, DISPLAY_CMD);   //SET COLUMN ADDRESS MSB 0
    display_send (0x00, DISPLAY_CMD);   //SET COLUMN ADRESS LSB 0
}

void
display_set_pixel (uint8_t x, uint8_t y, bool value)
{
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
        return;

    uint8_t page = y / 8;
    uint8_t col = DISPLAY_WIDTH - x - 1;
    uint8_t bit_index = 7 - (y % 8);

    if (value ^ inverted)
        backbuffer[page][col] |= 1 << bit_index;    // Black pixel
    else
        backbuffer[page][col] &= ~(1 << bit_index); // White pixel
}

inline static void
display_send (uint8_t value, uint8_t data)
{
    clear_bit (PORTH.OUT, DISPLAY_CS);
    clear_bit (PORTH.OUT, DISPLAY_WR);
    set_bit (PORTH.OUT, DISPLAY_RD);

    if (data == DISPLAY_DATA)
    {
        set_bit (PORTH.OUT, DISPLAY_RS);
        PORTF.OUT = value;
    }
    else
    {
        clear_bit (PORTH.OUT, DISPLAY_RS);
        PORTF.OUT =
            ((value & 0x01) << 7) | ((value & 0x02) << 5) | ((value & 0x04) << 3) |
            ((value & 0x08) << 1) | ((value & 0x10) >> 1) | ((value & 0x20) >> 3) |
            ((value & 0x40) >> 5) | ((value & 0x80) >> 7);
    }

    set_bit (PORTH.OUT, DISPLAY_WR);
    set_bit (PORTH.OUT, DISPLAY_CS);
}

inline static void
display_set_col (uint8_t col)
{
    display_send (0x10 + (col >> 4), DISPLAY_CMD);  // MSB
    display_send (0x00 + (col & 0x0F), DISPLAY_CMD);    // LSB
}

inline static void
display_set_page (uint8_t page)
{
    // Top down instead of bottom up
    display_send (0xB0 + page, DISPLAY_CMD);
}

void
display_flip (void)
{
    for (uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++)
    {
        display_set_page (page);
        display_set_col (0);
        for (uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++)
            display_send (backbuffer[page][col], DISPLAY_DATA);
    }
}

void
display_clear (void)
{
    for (uint8_t page = 0; page < DISPLAY_BACKBUFFER_LINES; page++)
        for (uint8_t col = 0; col < DISPLAY_BACKBUFFER_COLUMNS; col++)
            backbuffer[page][col] = 0;
}
