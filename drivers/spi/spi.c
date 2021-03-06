/**
 * SPI driver for XMega
 * @file spi.c
 * @author Maximilian Karl
 */
#include "spi.h"
#include <util/delay.h>

#define SPIUARTPORT USARTD0

#define SCK    7  // USART XCK
#define MISO   6  // USART RxD
#define MOSI   5  // USART TxD
#define ORIGCS 4  // original SPI chip select
#define CS     0  // just an output pin

#define SPIPORT PORTD
#define SPIMOD  SPID

/* Hardware defines */

#define SPI_SS_bm             0x10 /*!< \brief Bit mask for the SS pin. */
#define SPI_MOSI_bm           0x20 /*!< \brief Bit mask for the MOSI pin. */
#define SPI_MISO_bm           0x40 /*!< \brief Bit mask for the MISO pin. */
#define SPI_SCK_bm            0x80 /*!< \brief Bit mask for the SCK pin. */

/* SPI master status code defines. */

#define SPI_OK              0     /*!< \brief The transmission completed successfully. */
#define SPI_INTERRUPTED     1     /*!< \brief The transmission was interrupted by another master. */
#define SPI_BUSY            2     /*!< \brief The SPI module is busy with another transmission. */

void spi_wait_busy(void)
{
    uint8_t timeout = 200;

    while (!(SPIMOD.STATUS & _BV(SPI_IF_bp)) && timeout > 0)
        timeout--;

    if (timeout == 0)
	debug_pgm(PSTR("spi timeout reached!"));
}

uint8_t sd_raw_xmit_byte (uint8_t b)
{
    /* Send pattern. */
    SPIMOD.DATA = b;

    /* Wait for transmission complete. */
    //debug_pgm(PSTR("Wait for transmission complete"));
    spi_wait_busy();

    //debug_pgm(PSTR("transmission complete"));
    /* Read received data. */
    return SPIMOD.DATA;
}
#define nop() \
   asm volatile ("nop")
void spi_init()
{
    //debug_pgm (PSTR ("spi_init"));

    SPIPORT.DIR |= 0xB0;
    set_bit (SPIPORT.DIR, CS);


    /* enable spi, set master and clock modes (f/8) */
	SPIMOD.CTRL = _BV(SPI_MASTER_bp);
	SPIMOD.CTRL |= _BV(SPI_ENABLE_bp) |
	            	//_BV(SPI_CLK2X_bp)  |
	            	_BV(SPI_PRESCALER0_bp) | _BV(SPI_PRESCALER1_bp);

	return;

    /* Init SS pin as output with wired AND and pull-up. */
    SPIPORT.DIRSET = PIN4_bm;
    SPIPORT.PIN4CTRL = PORT_OPC_WIREDANDPULL_gc;

    /* Set SS output to high. (No slave addressed). */
    SPIPORT.OUTSET = PIN4_bm;


    //set_bit(SPIPORT.OUT, CS);


    set_bit (SPIPORT.DIR, MOSI);
    PORTD.PIN5CTRL = PORT_OPC_WIREDANDPULL_gc;
    set_bit (SPIPORT.DIR, CS);
    //set_bit(SPIPORT.DIR, ORIGCS);
    clear_bit (SPIPORT.DIR, MISO);
    PORTD.PIN6CTRL = PORT_OPC_WIREDANDPULL_gc;
    set_bit (SPIPORT.DIR, SCK);
    PORTD.PIN7CTRL = PORT_OPC_WIREDANDPULL_gc;
    PORTD.PIN0CTRL = PORT_OPC_WIREDANDPULL_gc;

    set_bit (SPIPORT.OUT, CS);
    set_bit (SPIPORT.OUT, ORIGCS);

	uint8_t a=0;
	for(a=0;a<200;a++){
	nop();
	};		//Wartet eine kurze Zeit

    SPIMOD.CTRL   = SPI_PRESCALER_DIV128_gc |                  /* SPI prescaler. */
                    (false ? SPI_CLK2X_bm : 0) |     /* SPI Clock double. */
                    SPI_ENABLE_bm |                  /* Enable SPI module. */
                    (false ? SPI_DORD_bm  : 0) |  /* Data order. */
                    SPI_MASTER_bm |                  /* SPI master. */
                    SPI_MODE_0_gc;                   /* SPI mode. */

    /* Interrupt level. */
    SPIMOD.INTCTRL = SPI_INTLVL_OFF_gc;

    //SPIPORT.DIRSET  = SPI_MOSI_bm | SPI_SCK_bm;








    /*// Assert low on SS - SPI connected to SPIPORT
    //clear_bit(SPIPORT_OUT, 4);
    set_bit(SPIPORT.OUT, 4); // Sets Slave Select (SS) as Output
    set_bit(SPIPORT.OUT, 5); // Set MOSI and SCK as Output
    set_bit(SPIPORT.OUT, 7);
    set_bit(SPID.CTRL, 6); // Enable SPI
    set_bit(SPID.CTRL, 4); // Set to Master
    set_bit(SPID.CTRL, 7); // CLK2X
    //clear_bit(SPID.CTRL, 0); // Prescaler to 00 (by default)
    //clear_bit(SPID.CTRL, 1); // Results in SCK frequency = clk_PER/2*/

    /*// CS und MOSI High
    set_bit(SPIPORT.OUT, CS); // nicht aktiv
    set_bit(SPIPORT.OUT, MOSI);
    // CLK Low
    clear_bit(SPIPORT.OUT, SCK);

    // CS, CLK und MOSI als Ausgänge
    set_bit(SPIPORT.DIRSET, MOSI);
    set_bit(SPIPORT.DIRSET, SCK);
    set_bit(SPIPORT.DIRSET, CS);

    // MISO als Eingang
    set_bit(SPIPORT.DIRCLR, MISO);

    // 100Khz SPI Frequenz zur Initialisierung
    // BSEL = 32e6 / (2 * Baudrate) - 1
    // BSEL = 2e6 / (2 * Baudrate) - 1
    SPIUARTPORT.BAUDCTRLA = 159;
    SPIUARTPORT.BAUDCTRLB = 0;

    // Frameformat (MSB first), SPI Mode 0
    SPIUARTPORT.CTRLC = USART_CMODE_MSPI_gc;

    SPIUARTPORT.CTRLA = 0;

    // enable RX und TX
    SPIUARTPORT.CTRLB = USART_RXEN_bm | USART_TXEN_bm;*/


}

void spi_send_byte (uint8_t byte_to_send)
{
    sd_raw_xmit_byte (byte_to_send);	
}

uint8_t spi_receive_byte()
{
    return sd_raw_xmit_byte (0xFF);
}

