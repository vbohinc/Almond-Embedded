/*
      ___                         ___           ___           ___          _____    
     /  /\                       /__/\         /  /\         /__/\        /  /::\   
    /  /::\                     |  |::\       /  /::\        \  \:\      /  /:/\:\  
   /  /:/\:\    ___     ___     |  |:|:\     /  /:/\:\        \  \:\    /  /:/  \:\ 
  /  /:/~/::\  /__/\   /  /\  __|__|:|\:\   /  /:/  \:\   _____\__\:\  /__/:/ \__\:|
 /__/:/ /:/\:\ \  \:\ /  /:/ /__/::::| \:\ /__/:/ \__\:\ /__/::::::::\ \  \:\ /  /:/
 \  \:\/:/__\/  \  \:\  /:/  \  \:\~~\__\/ \  \:\ /  /:/ \  \:\~~\~~\/  \  \:\  /:/ 
  \  \::/        \  \:\/:/    \  \:\        \  \:\  /:/   \  \:\  ~~~    \  \:\/:/  
   \  \:\         \  \::/      \  \:\        \  \:\/:/     \  \:\         \  \::/   
    \  \:\         \__\/        \  \:\        \  \::/       \  \:\         \__\/    
     \__\/                       \__\/         \__\/         \__\/                  
                    ___           ___            
     _____         /__/\         /  /\           
    /  /::\       |  |::\       /  /::\          
   /  /:/\:\      |  |:|:\     /  /:/\:\         
  /  /:/~/::\   __|__|:|\:\   /  /:/~/:/         
 /__/:/ /:/\:| /__/::::| \:\ /__/:/ /:/          
 \  \:\/:/~/:/ \  \:\~~\__\/ \  \:\/:/           
  \  \::/ /:/   \  \:\        \  \::/            
   \  \:\/:/     \  \:\        \  \:\            
    \  \::/       \  \:\        \  \:\           
     \__\/         \__\/         \__\/           
*/

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
#ifdef __BMP085_H__
#define __BMP085_H__

struct bmp_data {
  int32_t temprature;
  int32_t pressure;
}

extern void init_bmp085_sensor(void);

struct bmp_data bmp085_get_data(void);
#endif //__BMP085_H__

