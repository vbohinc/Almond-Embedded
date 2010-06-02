#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>

#define CRC32POLYREV 0xEDB88320 /* CRC-32 Polynom, umgekehrte Bitfolge */

extern void crc_32( uint32_t* crc32, uint8_t* buffer, uint8_t length );

#endif //_CRC_H_
