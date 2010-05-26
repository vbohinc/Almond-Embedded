/**
* @file storage.h
* @author Christian Rupprecht
* @date 05.05.2010
*
* This is the header file for the storage subsystem on the Squirrel for addressing SD Cards.
*
*/

#include <stdbool.h>
#include <stdint.h>
/**
* Writes a sensor value to the log-file.
* @return True if successful.
*/
bool storage_save_log(uint8_t device_id, uint8_t sensor_id, uint8_t value, uint8_t date);

/**
* Opens a log file for reading.
* @return True if successful.
*
*/
bool storage_open_log(uint8_t device_id, uint8_t sensor_id);

/**
* Pulls a section of the open log file sequentially.
* Repeated calls of this function will yield the entire file.
* @param file_buffer The buffer to write the section to.
* @param buffer_size Size of the buffer passed
* @return Size of data read. If /= buffer_size, EOF encountered.
*/
uint8_t storage_pull_log(&file_buffer, uint8_t buffer_size);

/**
* Pulls uplink packages from the open log file sequentially.
* Repeated calls of this function will yield the entire file.
* @param package_buffer The buffer to write the section to.
* @return True if successful.
*/
bool storage_pull_log_uplink_package(&package_buffer);

/**
* Sets the given value for the key indicated.
* @param key Key of the configuration value to set
* @value value The value to set
* @return True if successful.
*/
bool storage_set_configuration(uint8_t key, uint8_t value);

/**
* Returns the configuration value for a certain key.
* @param key Key of the requested configuration value
* @return Configuration value for given key.
*/
uint8_t storage_get_configuration(uint8_t key);


/**
* Closes the currently opened log file.
* @return True if successful.
*/
bool storage_close_log();

/**
* Formats the SD Card using FAT32.
* @return True if successful
*/
bool storage_format();
