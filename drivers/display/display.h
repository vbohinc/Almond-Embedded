#include <avr/io.h>
#include <util/delay.h>
#include "../../shared/common.h"

/**
* @file display_api.c
* @author parsch
* @date 2010/06/2
*
* The display API
*/

#define DISPLAY_PAGE_INIT 7

#define DISPLAY_COL_INIT 4

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

/**
 * Sends a command to the display
 * @param command command to be sent
 */
void display_command(uint8_t command);

/**
 * Writes a value to the lcd-display
 * @param value value to be written
 */
void display_write(uint8_t value);

/**
 *  Sets Column
 *  @param col column to be set
 */
void display_set_col(uint8_t col);

/**
 * Sets Page
 * @param page page to be set
 */
void display_set_page(uint8_t page);

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




