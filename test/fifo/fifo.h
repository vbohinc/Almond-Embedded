/**
* @file fifo.h
* @author Pascal Schnurr
* @date 23.09.2010
*
* This is the new fifo implementation for the blueooth rewrite.
*
*/

//#include <common.h>
//#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
typedef char prog_char;


#ifndef _FIFO_H_
#define _FIFO_H_

typedef struct
{
  uint8_t count;
  uint8_t head;
  uint8_t size;
  uint8_t* buffer;
} fifo_t;

void fifo_init (fifo_t *fifo, uint8_t *buffer, uint8_t size);
bool fifo_is_empty (const fifo_t *fifo);
bool fifo_is_full (const fifo_t *fifo);
bool fifo_clear (fifo_t *fifo);
bool fifo_read (fifo_t *fifo, char *data);
bool fifo_write (fifo_t *fifo, const char data);
bool fifo_cmp_pgm (fifo_t* fifo, const prog_char* pgm);
bool fifo_strstr_pgm (fifo_t *fifo, const prog_char *pgm);

#endif /* _FIFO_H_ */
