#include "display_api.h"

void display_command(unsigned char command) {
    set_low(PORTB, DISPLAY_RS);
    set_low(PORTE, DISPLAY_CS);
    set_low(PORTE, DISPLAY_WR);
    set_high(PORTB, DISPLAY_RD);
    PORTA = command;
    set_high(PORTE, DISPLAY_WR);
    set_high(PORTE, DISPLAY_CS);
}

void display_write(unsigned char value) {
    set_high(PORTB, DISPLAY_RS);
    set_low(PORTE, DISPLAY_CS);
    set_low(PORTE, DISPLAY_WR);
    set_high(PORTB, DISPLAY_RD);
    PORTA = value;
    set_high(PORTE, DISPLAY_WR);
    set_high(PORTE, DISPLAY_CS);
}

void display_set_col(unsigned char col){
    // MSB
    display_command(0x10 + (col>> 4));
    // LSB
    display_command(0x00 + (col & 0x0F));
}

void display_set_page(unsigned char page) {
	display_command(0xB0 + page);
}

void display_init(void) {
	//According to the init routine in the manual

	//User system setup by external pins
    DDRA = 0xFF;
    set_high(DDRB, DDB3);
    set_high(DDRC, DDC7);
    set_high(DDRE, DDE0);
    set_high(DDRE, DDE1);

    set_high(PORTE, DISPLAY_CS);

    //RST Low
    set_low(PORTC, DISPLAY_RST);

    //System setup by external pins
    set_high(PORTB, DISPLAY_RS);
    set_high(PORTE, DISPLAY_WR);
    set_high(PORTB, DISPLAY_RD);

    //Waiting for stabilizing power
    _delay_ms(100);

    //RST high
    set_high(PORTC, DISPLAY_RST);


    //ADC SELECT REVERSE
    display_command(0xA1);
    //SHL Select REVERSE
    display_command(0xC8);

    //LCD Bias Select
    display_command(0xA2);


    //PowerControl
    display_command(0x25);
//-----------------------------------------
    //Set reference voltage mode
    display_command(0x81);
    //Set reference voltage register
    display_command(0x30);
//------------------------------------
    //PowerControl
    display_command(0x2F);


//DATA DISPLAY PART

    //Initial Display line to 0
    display_command(0x40);
    // SET PAGE ADDRESS 0
    display_command(0xB0);
    //SET COLUMN ADDRESS MSB 0
    display_command(0x10);
    //SET COLUMN ADRESS LSB 0
    display_command(0x00);

    //REVERSE DISPLAY OFF
    display_command(0xA6);
    // Display ON
    display_command(0xAF);

    //Set initial column and page
    display_set_col(DISPLAY_COL_INIT);
    display_set_page(DISPLAY_PAGE_INIT);


}

void display_clean(void) {
	int i;
	for (i=0; i<8; i++){
		display_set_page(i);
		display_set_col(0);
		int j;
		for (j=0; j<132; j++) {
			display_write(0x00);
		}

	}
    display_set_col(DISPLAY_COL_INIT);
    display_set_page(DISPLAY_PAGE_INIT);
}






