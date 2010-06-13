#include "display.h"

void display_command(uint8_t command) {
    clear_bit(PORTB, DISPLAY_RS);
    clear_bit(PORTE, DISPLAY_CS);
    clear_bit(PORTE, DISPLAY_WR);
    set_bit(PORTB, DISPLAY_RD);
    PORTA = command;
    set_bit(PORTE, DISPLAY_WR);
    set_bit(PORTE, DISPLAY_CS);
}

void display_write(uint8_t value) {
    set_bit(PORTB, DISPLAY_RS);
    clear_bit(PORTE, DISPLAY_CS);
    clear_bit(PORTE, DISPLAY_WR);
    set_bit(PORTB, DISPLAY_RD);
    PORTA = value;
    set_bit(PORTE, DISPLAY_WR);
    set_bit(PORTE, DISPLAY_CS);
}

void display_set_col(uint8_t col){
    // MSB
    display_command(0x10 + (col>> 4));
    // LSB
    display_command(0x00 + (col & 0x0F));
}

void display_set_page(uint8_t page) {
	display_command(0xB0 + page);
}

void display_init(void) {
	//According to the init routine in the manual

	//User system setup by external pins
    DDRA = 0xFF;
    set_bit(DDRB, DDB3);
    set_bit(DDRC, DDC7);
    set_bit(DDRE, DDE0);
    set_bit(DDRE, DDE1);

    set_bit(PORTE, DISPLAY_CS);

    //RST Low
    clear_bit(PORTC, DISPLAY_RST);

    //System setup by external pins
    set_bit(PORTB, DISPLAY_RS);
    set_bit(PORTE, DISPLAY_WR);
    set_bit(PORTB, DISPLAY_RD);

    //Waiting for stabilizing power
    _delay_ms(100);

    //RST high
    set_bit(PORTC, DISPLAY_RST);


    //ADC SELECT REVERSE
    display_command(0xA1);
    //SHL Select REVERSE
    display_command(0xC8);

    //LCD Bias Select
    display_command(0xA2);


    //Regulator resistor select
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
	uint8_t i;
	for (i=0; i<8; i++){
		display_set_page(i);
		display_set_col(0);
		uint8_t j;
		// only 128 visible
		for (j=0; j<132; j++) {
			display_write(0x00);
		}

	}
    display_set_col(DISPLAY_COL_INIT);
    display_set_page(DISPLAY_PAGE_INIT);
}






