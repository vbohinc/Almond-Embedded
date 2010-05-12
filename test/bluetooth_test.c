/*
 * main.c
 *
 *  Created on: 02.05.2010
 *      Author: stefan
 */

#include "bluetooth/bluetooth.h"

void bluetooth_callback_handler(uint8_t *data_package, const uint8_t callback_type)
{

}

int main(void)
{
	bluetooth_init(bluetooth_callback_handler);
}
