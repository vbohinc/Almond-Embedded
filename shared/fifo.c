#include "fifo.h"


void
fifo_init (fifo_t* fifo, uint8_t* buffer, uint8_t size)
{
  fifo->size = size;
  fifo->buffer = buffer;
  fifo->head = 0;
  fifo->count = 0;
}

bool
fifo_is_empty(fifo_t* fifo)
{
  return (fifo->count == 0);
}

bool 
fifo_is_full(fifo_t* fifo)
{
  return (fifo->size - fifo->count <= 0);
}

bool 
fifo_read(fifo_t* fifo, uint8_t* data, uint8_t length)
{
  if (!fifo_is_empty(fifo))
    {
      if (length < fifo->count)
        {
          uint8_t* output = data;
          for (uint8_t i=0; i<length; i++)
            {
              fifo->head = ((fifo->head + i)%fifo->size);
              uint8_t* head = fifo->buffer + fifo->head;
              *output = *head;
              fifo->count--;
            }
          return true;
        }
    }
  return false;
}


bool 
fifo_read_c(fifo_t* fifo, char* data)
{
  if (!fifo_is_empty (fifo))
    {
      uint8_t* head = fifo->buffer + fifo->head;
      *data = (char) *head;
      fifo->head = ((fifo->head + 1)%fifo->size);
      fifo->count--;
      return true;
    }
  return false;
}

bool 
fifo_write_c(fifo_t* fifo, char data)
{
  if (!fifo_is_full(fifo))
    {
      uint8_t* end = fifo->buffer + ((fifo->head + fifo->count)%fifo->size);
      *end = (uint8_t) data;
      fifo->count++;
      return true;
    }
  return false;
}

bool 
fifo_clear(fifo_t* fifo)
{
  fifo->count = 0;
  fifo->head = 0;
  return true;
}

bool 
fifo_write_p(fifo_t* fifo, prog_char* progmem)
{
  if (!fifo_is_full(fifo))
    {
      uint8_t i = 0;
      uint8_t* end;
      while (pgm_read_byte(progmem+i) != 0)
        {
          if (fifo_is_full(fifo))
            return false;
          end = fifo->buffer + ((fifo->head + fifo->count)%fifo->size);
          *end = pgm_read_byte(progmem+i);
          fifo->count++;
          i++;
        }
      return true;
    }
  return false;
}

bool 
fifo_write_s (fifo_t* fifo, const char* data)
{
  if (!fifo_is_full(fifo))
    {
      uint8_t i = 0;
      uint8_t* end;
      while (data[i] != 0)
        {
          if (fifo_is_full(fifo))
            return false;
          end = fifo->buffer + ((fifo->head + fifo->count)%fifo->size);
          *end = data[i];
          fifo->count++;
          i++;
        }
      return true;
    }
  return false;
}

bool 
fifo_cmp_pgm(fifo_t* fifo, prog_char* progmem)
{
  if (!fifo_is_empty(fifo))
    {
      prog_char* pgm = progmem;
      uint8_t i = 0;
      uint8_t head = 0;
      uint8_t* head_p;
      uint8_t count = fifo->count;
      while (pgm_read_byte(pgm) != 0)
        {
          head = ((fifo->head + i)%fifo->size);
          head_p = fifo->buffer + head;
          if (count <= 0 || *head_p != (uint8_t) *pgm)
            return false;
          pgm++;
          i++;
          count--;
        }
      //we found something, in that case move pointer
      fifo->head = ((fifo->head + i)%fifo->size);
      fifo->count = count;
      return true;
    }
  return false;
}
