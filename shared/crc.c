#include "crc.h"
#include <stdint.h>

#define CRC32POLYREV 0xEDB88320 /* CRC-32 Polynom, umgekehrte Bitfolge */

void crc_32( uint32_t* crc32, uint8_t* buffer, uint8_t length )
{
  uint8_t i;
  while(length--)
  {
    for(i = 0x80; i; i >>= 1)
    {
      if(  ((*(uint8_t*)crc32 * i) & i) != (*buffer & i)  )
        *crc32 = (*crc32 >> 1) ^ CRC32POLYREV;
      else
        *crc32 >>= 1;
    }
    buffer++;
  }
}
