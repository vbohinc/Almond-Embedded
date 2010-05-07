/**
 * @brief sensor driver for the bmp085 pressure and temprature sensor
 * 
 */

/**
 * requires an initialized and working twi bus, will connect and retrieve data and afterwards convert it
 * @param temprature a pointer to the temprature data
 * @param pressure a pointer to the pressure data
 * @return 1 on failure 0 otherwise
 */
uint8_t bmp085_get_data(uint16_t * temprature, uint16_t * pressure);
