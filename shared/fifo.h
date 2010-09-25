/**
* @file fifo.h
* @author Pascal Schnurr
* @date 23.09.2010
*
*  This is the new fifo implementation for the blueooth rewrite.
*
*/
#include <avr/pgmspace.h>
#include <common.h>

#ifndef _FIFO_H_
#define _FIFO_H_

typedef struct
{
  uint8_t size;
  uint8_t count;
  uint8_t* buffer;
  uint8_t head;
} fifo_t;

void fifo_init(fifo_t* new_fifo,uint8_t* buffer, uint8_t size);

bool fifo_is_empty (fifo_t* fifo);
bool fifo_is_full (fifo_t* fifo);

bool fifo_cmp_pgm (fifo_t* fifo, prog_char* programm);

bool fifo_read (fifo_t* fifo, char* data);

bool fifo_write (fifo_t* fifo, const char data);

bool fifo_clear (fifo_t* fifo);

#endif /* _FIFO_H_ */
