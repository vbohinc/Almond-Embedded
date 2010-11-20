/**
 * fifo.h - a simple Fifo
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 * \Author Pascal Schnurr
 */

#include <common.h>
#include <avr/pgmspace.h>

#ifndef _FIFO_H_
#define _FIFO_H_

typedef struct
{
    uint16_t count;
    uint16_t head;
    uint16_t size;
    uint8_t* buffer;
} fifo_t;

void fifo_init (fifo_t *fifo, uint8_t *buffer, uint16_t size);
bool fifo_is_empty (const fifo_t *fifo);
bool fifo_is_full (const fifo_t *fifo);
bool fifo_clear (fifo_t *fifo);
bool fifo_read (fifo_t *fifo, char *data);
bool fifo_write (fifo_t *fifo, const char data);
bool fifo_cmp_pgm (fifo_t* fifo, const prog_char* pgm);
bool fifo_strstr_pgm (fifo_t *fifo, const prog_char *pgm);

#endif /* _FIFO_H_ */
