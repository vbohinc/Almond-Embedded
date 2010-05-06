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

int main()
{
	bluetooth_init();

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

	uint8_t *found = bluetooth_cmd_search_devices();
	if (found == NULL)
		printf("SEARCH ERROR!");
	else {
		int count = found[0];
		printf("\nCount=%d", count);
		for (int i=0; i<count; i++)
		{
			printf("\n%d: '", i);
			for (int j=0; j<16; j++)
			{
				printf("%c", found[1+i*(16+12)+j]);
			}
			printf("' ");
			for (int j=0; j<12; j++)
				printf("%c", found[1+i*(16+12)+16+j]);
		}
	}

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


	/*uint8_t *arr = bluetooth_cmd_get_address();

	while(1);

	int i=0;

	while (arr[i]!=0)
		printf("%d:%c\n",i,arr[i]);*/

	bluetooth_close();
}

