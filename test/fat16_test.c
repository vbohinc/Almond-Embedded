#ifndef X86

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define UART_BAUD_RATE      9600ul

#else

#include <stdbool.h>
#include <SDL.h> // main SDL header
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>

#endif

#include "../../drivers/display/display.h"
#include "../../drivers/display/display_draw.h"
#include "../../drivers/display/display_gui.h"

#include "../../drivers/fat16/fat16.h"

#include "../../drivers/display/display_data.h"
#include "../pong.h"

/*! Define that selects the Usart used in example. */
#define USART USARTC0


void log(const char* msg) {
    display_print(msg);
    display_print("\n");
}


// Fat16 test for AVR
int main (void)
{
    //NOT NEEDED ?----------------------------------------
  /* Behold the MAGIC Clock! */

  /* Internen 32Mhz Oszillator einschalten */
  OSC.CTRL = OSC_RC32MEN_bm;

  /* Warten bis Oszillator stabil ist */
  while ((OSC.STATUS & OSC_RC32MRDY_bm) == 0);

  /* System Clock selection */
  CCP = CCP_IOREG_gc;
  CLK.CTRL = CLK_SCLKSEL_RC32M_gc;

  /* DFLL ein (Auto Kalibrierung) */
  DFLLRC32M.CTRL = DFLL_ENABLE_bm;


	set_bit(PORTC.DIR,4);
	clear_bit(PORTC.OUT,4);

	set_bit(PORTH.DIR,2);
	set_bit(PORTH.OUT,2);
	//END NEEDED?---------------------------------------

	if(fat16_init(0) != 1) {
	    log("ERROR: FAT16 init failed!");
	    return -1;
	}

	fat16_dir_entry* test_dir;

	if(fat16_get_dir_entry_of_path("test", test_dir) == 0) {
	    if(fat16_write_dir_entry(test_dir) == 0) {
	        log("ERROR: creation failed test/");
	        return -1;
	    }
	}

	fat16_dir_entry* test_file;

    if(fat16_create_file(test_dir,"testfile",test_file)) {
        log("ERROR: creation failed test/testfile");
        return -1;
    }

    fat16_file* test_file_fd;

    if(fat16_open_file(test_file_fd,test_file) == 0) {
        log("ERROR: opening file test/testfile");
        return -1;
    }


    uint8_t buffer[128];

    for(int i = 0; i<128; i++) {
        buffer[i] = i;
    }

    if(fat16_write_file(test_file_fd,&buffer,128) == 0) {
        log("ERROR: writing to file");
        return -1;
    }

    for(int i = 0; i<128; i++) {
        buffer[i] = 0;
    }

    if(fat16_read_file(test_file_fd,&buffer,128) == 0) {
        log("ERROR: failed to write file test/testfile");
        return -1;
    }

    for(int i = 0; i<128; i++) {
        if(buffer[i] != i) {
            log("ERROR: write/read data mismatch!");
            return -1;
        }
    }

    fat16_dir* test_dir_dd;

    if(fat16_open_dir(test_dir_dd,test_dir) == 0) {
        log("ERROR:failed to open dir test");
        return -1;
    }

    fat16_dir_entry* entry;

    if(fat16_read_dir(test_dir_dd,entry) == 0) {
        log("ERROR: failed to read dir entrys");
        return -1;
    }

    log("Content of test folder:");
    log(entry->long_name);

    if(fat16_reset_dir(test_dir_dd) == 0) {
        log("ERROR: failed to reset dir test");
    }

    if(fat16_delete_file(test_file) == 0) {
        log("ERROR: deleting file test/festfile");
        return -1;
    }

    if(fat16_delete_file(test_dir) == 0) {
        log("ERROR: deleting folder test");
        return -1;
    }

    log("FINISHED WITHOUT ERROR!");
    return 1;
}










