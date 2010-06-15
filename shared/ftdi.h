#ifndef _FTDI_H_
#define _FTDI_H_

#include <avr/io.h>
#include <util/delay.h>

#define nop() \
   asm volatile ("nop");

#define P_RD  PB2
#define P_WR  PB3
#define P_TXE PB0
#define P_RXF PB1
#define P_SI  PB4

#define DD_RD  DDB2 // 1
#define DD_WR  DDB3 // 1
#define DD_TXE DDB0 // 0
#define DD_RXF DDB1 // 0
#define DD_SI  DDB4 // 1

#define TXE (PINB & (1<<P_TXE))
#define RXF (PINB & (1<<P_RXF))

#define FTDI_DD ( (1 << DD_RD) | (1 << DD_WR) | (1 << DD_SI) )

extern void FTDIInit(void);

extern void FTDISend( uint8_t out_buf);

extern void FTDISendImmediate(void);

extern uint8_t FTDIRead( uint8_t *out_buf);

#endif /* _FTDI_H_ */
