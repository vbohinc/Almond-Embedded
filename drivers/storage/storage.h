/**
* @file storage.h
* @author Christian Rupprecht
* @date 05.05.2010
*
* This is the header file for the storage subsystem on the Squirrel for addressing SD Cards.
*
*/

/**
* Writes a sensor value to the log-file.
* @return True if successful.
*/
bool storage_save_log(uint_8 device_id, uint_8 sensor_id, uint_8 value, uint_8 date);

/**
* Opens a log file for reading.
* @return True if successful.
*
*/
bool storage_open_log(uint_8 device_id, uint_8 sensor_id);

/**
* Pulls a section of the open log file sequentially.
* Repeated calls of this function will yield the entire file.
* @param file_buffer The buffer to write the section to.
* @param buffer_size Size of the buffer passed
* @return Size of data read. If /= buffer_size, EOF encountered.
*/
uint_8 storage_pull_log(&file_buffer, uint_8 buffer_size);

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
bool storage_set_configuration(uint_8 key, uint_8 value);

/**
* Returns the configuration value for a certain key.
* @param key Key of the requested configuration value
* @return Configuration value for given key.
*/
uint_8 storage_get_configuration(uint_8 key);


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