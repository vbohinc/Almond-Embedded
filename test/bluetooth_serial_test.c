/*
* @file bluetooth_serial_test.c
* @author Stefan Profanter
* @date 06.05.2010
*
* This file is used on the PC to send commands over the /dev/ttysUSB terminal to the BTM-222 Module.
* It's purpose is to test command handling. So don't compile this file for embedded system.
*
*/
#include "drivers/bluetooth/bluetooth.h"

#include <stdio.h>
#include <time.h>
#include <string.h>

void bluetooth_callback_handler(uint8_t *data_package, const uint8_t callback_type)
{
	if (callback_type == 0) {//is data package
		printf("Data Package received:\n");
		printf("[");
		for (int i=0; i<BLUETOOTH_DATA_PACKAGE_SIZE; i++)
		{
			printf("%d:%d,", i, data_package[i]);
		}
		printf("]\n");
	} else if (callback_type == 1) //connected
	{
		printf("Connected to: %s\n", data_package);
	} else if (callback_type == 2) //disconnected
	{
		printf("Disconnected from: %s\n", data_package);
	} else
		printf("Invalid callback type: %d\n", callback_type);
}


int main()
{
	bluetooth_init(bluetooth_callback_handler);

	char buffer[255];
	uint8_t cmd[255];

	for (int i=0; i<4; i++)
	{
		int retval = bluetooth_cmd_test_connection();
		if (retval != 1)
			printf("\nAT%d ret=%d",i, retval);
		else
			break;
	}

	/*
	uint8_t* address = bluetooth_cmd_get_address();
	if (address == NULL)
		printf("ADDRESS ERROR");
	else
		printf("BT-Address: %s", address);
	*/

	/*
	//int retval = bluetooth_cmd_set_remote_address("1234AB789CDE");

	int retval = bluetooth_cmd_set_remote_address((uint8_t*)NULL);
	printf("\nATD ret=%d", retval);
	*/

	int retval = bluetooth_cmd_set_remote_address("701A041CDBF1");

	printf("\nATD ret=%d",retval);

	/*uint8_t *found = bluetooth_cmd_search_devices();
	if (found == NULL)
		printf("SEARCH ERROR!");
	else {
		int count = found[0];
		printf("\nCount=%d", count);
		for (int i=0; i<count; i++)
		{
			printf("\n%d: '", i+1);
			for (int j=0; j<16; j++)
			{
				printf("%c", found[1+i*(16+12)+j]);
			}
			printf("' ");
			for (int j=0; j<12; j++)
				printf("%c", found[1+i*(16+12)+16+j]);
		}
	}*/

	retval = bluetooth_cmd_connect(0);
	printf("\nATA ret=%d", retval);

	if (retval == 0)
	{
		retval = bluetooth_cmd_connect(1);
		printf("\nATA ret=%d", retval);
	}


	for (int i=0; i<BLUETOOTH_DATA_PACKAGE_SIZE; i++)
	{
		bluetooth_data_package[i]=i+48;
	}


	/*if (retval ==1)
	{
		retval = bluetooth_send_data_package(bluetooth_data_package, BLUETOOTH_DATA_PACKAGE_SIZE);
		printf("\nSend ret=%d", retval);

		retval = bluetooth_cmd_online_command();
		printf("\n+++ ret=%d", retval);

		retval = bluetooth_cmd_close_connection();
		printf("\nATH ret=%d", retval);

	}*/


	printf("Gib befehle ein:\n");

	while(1)
	{
		fgets(buffer, 255, stdin);
		if (strncmp(buffer, "exit",4)==0)
			break;
		int count = 0;
		while(buffer[count]!=0)
		{
			cmd[count] = buffer[count];
			count++;
		}
		if (strncmp(buffer, "+\n", 2)==0)
			count--;
		else
			cmd[count-1] = 13;

		cmd[count] = 0;

		if (bluetooth_cmd_send(cmd, 10)==0)
			perror("Couldn't send cmd");
	}

	bluetooth_close();
}

