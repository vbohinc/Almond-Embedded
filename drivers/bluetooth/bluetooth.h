/**
 * header for the Bluetooth driver
 * @file bluetooth.h
 * @author Linus Lotz<lotz@in.tum.de>
 * @author Salomon Sickert
 * @addtogroup bluetooth Bluetooth Driver
 */

/**@{*/
#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_

#include <avr/io.h>
#include <common.h>

///Bluetooth mode ENUM
typedef enum {
    BLUETOOTH_MASTER, ///< Master Mode (to create outgoinng connections).
    BLUETOOTH_SLAVE ///< Slavee Mode (to wait for incoming connections).  
} bt_mode_t;


/** init bluetooth driver
* @return always true
*/
extern bool bt_init (void);

/** Set the Bluetooth mode 
* @param mode bt_mode_t Bluetooth Mode ENUM (BLUETOOTH_MASTER or BLUETOOTH_SLAVE)
* @return true if mode change was succesful, false if not
*/
extern bool bt_set_mode (const bt_mode_t mode);

/** recieve data over bluetooth
* @param data pointer to memory for data storage
* @param length value of length after call holds the actual recived data length
* @param timeout_ms timeout in ms after the recive function aborts and returns with false
* @return false if recived length > length parameter or it timeouted, true otherwise  
*/
extern bool bt_receive (void * data, uint8_t *length, uint16_t timeout_ms);

/** send data over bluetooth
* @param data pointer to the data to send
* @param length length of the data to be send
* @return true if sendingn was successful, false otherwise
*/
extern bool bt_send (void * data, const uint8_t length);

///squirrelt only functions
#ifdef SQUIRREL

/** open bluetoot connection (only one at a time possible)
* @param address connection is opened to this device mac address
* @return true if connection was established, false otherwise
*/ 
extern bool bt_connect (const char *address);

/** closes bluetooth connection
*@return false if failed, true otherwise
*/
extern bool bt_disconnect (void);

/** discover bluetooth devices
* @param result in a 2 dimensional array first index are devicecs (max 8) second is mac address string
* @param update_callback to inform of progress (in % from 0 to 100)
* @return true if successful, false if error occured 
*/
extern bool bt_discover (char result[8][12], void (*update_callback)(const uint8_t progress));
#endif /* SQUIRREL */


#endif
/**@}*/
