#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <gui/gui_draw.h>
#include <sd/sd.h>
#include <fat16/fat16.h>

int main (void)
{
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

	display_init();
	error_init();
	display_flip();
	_delay_ms(10);
	sd_init();
	

    if(fat16_init(0) != 1) {
	    debug_pgm(PSTR("FAT16 init fail!"));
	    return -1;
	}

	struct fat16_file f;
    debug_pgm(PSTR("Los gehts"));

	if(fat16_open_file_by_name(&f, "/dei.txt") != 1)
	{
	    debug_pgm(PSTR("FAT16 file open fail!"));
	    return -1;
	}
	debug_pgm(PSTR("open file ok"));

    uint8_t buffer[1];
	while(fat16_read_file(&f,buffer,1) == 1)
    {
		debug(&buffer[0]);
        _delay_ms(250);
	}
	
#if 0

	if(fat16_init(0) != 1) {
	    logmsg("ERR: FAT16 init failed!");
	    return -1;
	}

	struct fat16_dir_entry test_dir;

	if (fat16_get_dir_entry_of_path("test", &test_dir) == 0) {
	    if(fat16_write_dir_entry(test_dir) == 0) {
	        logmsg("ERR: creation failed test/");
	        return -1;
	    }
	}

	fat16_dir_entry* test_file;
    if(fat16_create_file(test_dir,"testfile",test_file)) {
        logmsg("ERR: creation failed test/testfile");
        return -1;
    }

    fat16_file* test_file_fd;

    if(fat16_open_file(test_file_fd,test_file) == 0) {
        logmsg("ERR: opening file test/testfile");
        return -1;
    }


    uint8_t buffer[128];

    for(int i = 0; i<128; i++) {
        buffer[i] = i;
    }

    if(fat16_write_file(test_file_fd,buffer,sizeof(buffer)) == 0) {
        logmsg("ERR: writing to file");
        return -1;
    }

    for(int i = 0; i<128; i++) {
        buffer[i] = 0;
    }

    int32_t seek = 0;

    if(fat16_seek_file(test_file,&seek, FAT16_SEEK_SET)) {
        logmsg("ERR: seekt to 0 faile in test/testfile");
    }

    if(fat16_read_file(test_file_fd,buffer,sizeof(buffer)) == 0) {
        logmsg("ERR: failed to read file test/testfile");
        return -1;
    }

    for(int i = 0; i<128; i++) {
        if(buffer[i] != i) {
            logmsg("ERR: write/read data mismatch!");
            return -1;
        }
    }


    uint8_t buffer2[64];

    seek = 512;

    if(fat16_seek_file(test_file_fd,&seek,FAT16_SEEK_SET) == 0) {
        logmsg("ERR: seekt to 512 faile in test/testfile");
    }

    if(fat16_read_file(test_file_fd,buffer2,sizeof(buffer2)) == 0) {
        logmsg("ERR: failed to read file test/testfile");
        return -1;
    }

    for(int i = 0; i<64; i++) {
        if(buffer2[i] != i+64) {
            logmsg("ERR: write/read data mismatch!");
            return -1;
        }
    }

    fat16_dir* test_dir_dd;

    if(fat16_open_dir(test_dir_dd,test_dir) == 0) {
        logmsg("ERR:failed to open dir test");
        return -1;
    }

    fat16_dir_entry* entry;

    if(fat16_read_dir(test_dir_dd,entry) == 0) {
        logmsg("ERR: failed to read dir entrys");
        return -1;
    }

    logmsg("Content of test folder:");
    logmsg(entry->long_name);

    if(fat16_reset_dir(test_dir_dd) == 0) {
        logmsg("ERR: failed to reset dir test");
    }

    if(fat16_delete_file(test_file) == 0) {
        logmsg("ERR: deleting file test/festfile");
        return -1;
    }

    if(fat16_delete_file(test_dir) == 0) {
        logmsg("ERR: deleting folder test");
        return -1;
    }
#endif
    debug_pgm(PSTR("FINISHED WITHOUT ERROR!"));
    return 1;

}




