/*
 *  util.c
 *
 */
 
#include "util.h"

static uint32_t emu_time = 0;

bool time_set (uint32_t time)
{
  emu_time = time;
  return true;
}

uint32_t time_get ()
{
  return emu_time;
}

bool log_write (uint8_t* mac, uint8_t id, uint32_t time, uint16_t value)
{

#ifdef DEBUG
  debug ("Log write...");
#endif

  return true;
}

bool log_read (uint8_t* mac, uint8_t id, uint8_t page, uint8_t* buffer)
{

#ifdef DEBUG
  debug ("Log read...");
#endif

  return true;
}
