/**
 * utils for Squirrel
 * @file util.h
 */
#ifndef __UTIL_H__
#define __UTIL_H__

#include "../shared/common.h"

/**
*set the current time on squirrel
* @param time unix timestamp
* @return returns alwys true
*/
bool time_set (uint32_t time);

/**
*gets the current time on squirrel#
*@return current time as unixtimpestamp
*/
uint32_t time_get (void);

/**
*writes an log entry into log
*@param mac address othe the device
*@param id for creater sensor/part of the log entry
*@param time unixtimestamp 
*@param value to be logged
*@return true if writing succeded false otherwise
*/
bool log_write (char *mac, uint8_t id, uint32_t time, uint16_t value);

/**
*rads an log entry from log
*@param mac address of device to read log from
*@param id of the sensor/part of the device
*@param page that should be read
*@param buffer for the value read value
*@reutrn true if reaidng succeded false otherwise
*/
bool log_read (char *mac, uint8_t id, uint8_t page, uint8_t* buffer);

#endif
