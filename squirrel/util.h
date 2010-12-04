/**
 * util.h - utils for Squirrel
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */
#ifndef __UTIL_H__
#define __UTIL_H__

#include "../shared/common.h"

bool time_set (uint32_t time);
uint32_t time_get (void);

bool log_write (const char *mac, const uint8_t id, const uint32_t time, const uint16_t value);
bool log_read (const char *mac, const uint8_t id, const uint8_t page, uint8_t *buffer);

#endif
