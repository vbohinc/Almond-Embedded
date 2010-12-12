/**
 * a FTDI Debug output Driver
 * @file ftdi.c
 */

#include "ftdi.h"

void FTDIInit (void)
{
#ifndef __AVR_ATxmega128A1__
    // initialize data direction
    DDRD |= FTDI_DD1;
    DDRC |= FTDI_DD2;
    DDRB = 0xFF;

    PORTD |= (1 << P_SI);
#endif
}

void FTDISend (uint8_t out_buf)
{
#ifndef __AVR_ATxmega128A1__

    if (out_buf == '\0')
    {
        FTDISendImmediate();
        return;
    }

    while (TXE != 0)
        {};

    PORTB = out_buf;

    PORTC |= (1 << P_WR);

    nop();

    nop();

    PORTC &= ~ (1 << P_WR);

#endif
}

void FTDISendImmediate (void)
{
#ifndef __AVR_ATxmega128A1__
    // strobe low
    PORTD &= ~ (1 << P_SI);
    PORTD |= (1 << P_SI);
#endif
}

uint8_t FTDIRead (uint8_t *out_buf)
{
#ifndef __AVR_ATxmega128A1__
    // no data avaiable

    if (RXF)
        return 0;

    // set direction
    DDRB = 0x00;

    PORTD &= ~ (1 << P_RD);

    PORTD |= (1 << P_RD);

    // read byte out
    (*out_buf) = PINB;

    // set direction
    DDRB = 0xFF;


#endif
    return 1;
}
