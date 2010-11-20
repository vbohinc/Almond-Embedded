/**
 * fifo.c - a simple Fifo
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */
#include "fifo.h"

void
fifo_init (fifo_t * fifo, uint8_t * buffer, uint16_t size)
{
    fifo->size = size;
    fifo->buffer = buffer;
    fifo->head = 0;
    fifo->count = 0;
}

bool
fifo_is_empty (const fifo_t * fifo)
{
    return (fifo->count == 0);
}

bool
fifo_is_full (const fifo_t * fifo)
{
    return (fifo->size - fifo->count == 0);
}

bool
fifo_read (fifo_t * fifo, char *data)
{
    if (fifo_is_empty (fifo))
        return false;

    uint8_t *head = fifo->buffer + fifo->head;

    *data = (char) * head;

    fifo->head = ( (fifo->head + 1) % fifo->size);

    fifo->count--;

    return true;
}

bool
fifo_write (fifo_t * fifo, const char data)
{
    if (fifo_is_full (fifo))
        return false;

    uint8_t *end = fifo->buffer + ( (fifo->head + fifo->count) % fifo->size);

    *end = (uint8_t) data;

    fifo->count++;

    return true;
}

bool
fifo_clear (fifo_t * fifo)
{
    fifo->count = 0;
    fifo->head = 0;
    return true;
}

static bool
fifo_cmp_pgm_at (fifo_t * fifo, const prog_char * pgm, const uint16_t index)
{
    uint16_t i;
    uint16_t fifo_byte;
    uint16_t pgm_byte;

    for (i = 0; pgm_read_byte (pgm + i) != 0; i++)
    {
        if (fifo->count <= (i + index))
            return false;

        pgm_byte = pgm_read_byte (pgm + i);

        fifo_byte = * (fifo->buffer + ( (fifo->head + i + index) % fifo->size));

        if (fifo_byte != pgm_byte)
            return false;
    }

    // We found the string, lets move the pointer
    fifo->head = ( (fifo->head + i + index) % fifo->size);

    fifo->count -= (i + index);

    return true;
}

bool
fifo_cmp_pgm (fifo_t * fifo, const prog_char * pgm)
{
    return fifo_cmp_pgm_at (fifo, pgm, 0);
}

bool
fifo_strstr_pgm (fifo_t * fifo, const prog_char * pgm)
{
    for (uint16_t i = 0; i < fifo->count; i++)
    {
        if (fifo_cmp_pgm_at (fifo, pgm, i))
            return true;
    }

    return false;
}
