/**
* @file fifo.h
* @author Stefan Profanter
* @date 01.05.2010
*
* This file contains a representation of a FIFO list. The list can store only uint8_t data.
*
*/
#ifndef _FIFO_H_
#define _FIFO_H_

#ifdef SERIAL
#include <stdint.h>

#else
#include <avr/io.h>
#include <avr/interrupt.h>
#endif

/**
 * The FIFO.
 * Stores the data.
 */
typedef struct
{
	uint8_t volatile count;       /*! Number of bytes in fifo */
	uint8_t size;                 /*! Size of fifo (buffer) */
	uint8_t *pread;               /*! read pointer */
	uint8_t *pwrite;              /*! write pointer */
	uint8_t read2end, write2end;  /*! number of bytes to overflow of read/write pointer */
} fifo_t;

/**
 * Initialize a new FIFO.
 * Initializes a new FIFO with the given buffer and size.
 * @param fifo FIFO to initialize
 * @param buf The already initialized buffer array for the fifo with the size of 'size'
 * @param size Size of the Buffer.
 */
extern void fifo_init (fifo_t* fifo, uint8_t* buf, const uint8_t size);

/**
 * Put a byte into the fifo.
 * @param fifo Fifo to put byte into
 * @param data The data-byte to store
 */
extern uint8_t fifo_put (fifo_t* fifo, const uint8_t data);


/**
 * Get the first byte. If no byte available, wait until one will be put.
 * @param fifo Fifo to read from
 */
extern uint8_t fifo_get_wait (fifo_t* fifo);

/**
 * Get the first byte. If no byte available, returns -1.
 * @param fifo Fifo to read from
 */
extern int16_t fifo_get_nowait (fifo_t* fifo);

/**
 * Puts a byte into the fifo. Is called by fifo_put.
 * @param fifo Fifo to put byte into
 * @param data The data-byte to store
 */
static inline uint8_t _inline_fifo_put (fifo_t *fifo, const uint8_t data)
{
	if (fifo->count >= fifo->size)
		return 0;
		
	uint8_t * pwrite = fifo->pwrite;
	
	*(pwrite++) = data;
	
	uint8_t write2end = fifo->write2end;
	
	if (--write2end == 0)
	{
		write2end = fifo->size;
		pwrite -= write2end;
	}
	
	fifo->write2end = write2end;
	fifo->pwrite = pwrite;

#ifndef SERIAL
	uint8_t sreg = SREG;
	cli();
#endif
	fifo->count++;
#ifndef SERIAL
	SREG = sreg;
#endif
	
	return 1;
}

/**
 * Reads a byte from the fifo. Is called by fifo_get_nowait and fifo_get_wait.
 * @param fifo Fifo to read from
 */
static inline uint8_t _inline_fifo_get (fifo_t *f)
{
	uint8_t *pread = f->pread;
	uint8_t data = *(pread++);
	uint8_t read2end = f->read2end;
	
	if (--read2end == 0)
	{
		read2end = f->size;
		pread -= read2end;
	}
	
	f->pread = pread;
	f->read2end = read2end;
	
#ifndef SERIAL
	uint8_t sreg = SREG;
	cli();
#endif

	f->count--;
#ifndef SERIAL
	SREG = sreg;
#endif
	
	return data;
}

#endif /* _FIFO_H_ */
