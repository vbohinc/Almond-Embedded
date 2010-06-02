#include "fifo.h"

#ifdef SERIAL
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
#endif

void fifo_init (fifo_t *f, uint8_t *buffer, const uint8_t size)
{
	f->count = 0;
	f->pread = f->pwrite = buffer;
	f->read2end = f->write2end = f->size = size;
}

uint8_t fifo_put (fifo_t *f, const uint8_t data)
{
	return _inline_fifo_put (f, data);
}

uint8_t fifo_get_wait (fifo_t *f)
{
	while (!f->count);
	
	return _inline_fifo_get (f);	
}

int16_t fifo_get_nowait (fifo_t *f)
{
	if (!f->count)		return -1;
		
	return (uint8_t) _inline_fifo_get (f);
}
