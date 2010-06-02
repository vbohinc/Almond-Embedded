#include <avr/io.h>
#include <util/delay.h>

/**
* @file display_api.c
* @author parsch
* @date 2010/06/2
*
* The display API
*/

#define DISPLAY_PAGE_INIT 7

#define DISPLAY_COL_INIT 5

// chip select
#define DISPLAY_CS  PE0

// reset
#define DISPLAY_RST PC7

//Register Select input
#define DISPLAY_RS  PB3

//read
#define DISPLAY_RD  PB2

//write
#define DISPLAY_WR  PE1


// Macro to set a bit high on a certain port
#define set_high(port, bit)     ((port) |= (1 << (bit)))

 //Macro to set a bit low on certain port
#define set_low(port, bit)  ((port) &= ~(1 << (bit)))

/**
 * Sends a command to the display
 * @param command command to be sent
 */
void display_command(unsigned char command);

/**
 * Writes a value to the lcd-display
 * @param value value to be written
 */
void display_write(unsigned char value);

/**
 *  Sets Column
 *  @param col column to be set
 */
void display_set_col(unsigned char col);

/**
 * Sets Page
 * @param page page to be set
 */
void display_set_page(unsigned char page);

/**
*
* Initialization of the display
*
*/
void display_init(void);

/**
 * Cleans the display - nothing is to see anymore
 */
void display_clean(void);




