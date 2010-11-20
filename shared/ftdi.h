/**
 * ftdi.h - a FTDI Debug output Driver
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#ifndef _FTDI_H_
#define _FTDI_H_

#include <avr/io.h>
#include <util/delay.h>

#define nop() \
    asm volatile ("nop");

#define P_RD  PD6
#define P_WR  PC5
#define P_TXE PC3
#define P_RXF PC4
#define P_SI  PD5

#define DD_RD  DDD6 // 1
#define DD_WR  DDC5 // 1
#define DD_TXE DDC3 // 0
#define DD_RXF DDC4 // 0
#define DD_SI  DDD5 // 1

#define TXE (PINC & (1<<P_TXE))
#define RXF (PINC & (1<<P_RXF))

#define FTDI_DD1 ( (1 << DD_RD) | (1 << DD_SI) )
#define FTDI_DD2 ( (1 << DD_WR) )

extern void FTDIInit (void);

extern void FTDISend (uint8_t out_buf);

extern void FTDISendImmediate (void);

extern uint8_t FTDIRead (uint8_t *out_buf);

#endif /* _FTDI_H_ */
