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
#include <unistd.h>

int connected = 0;

void bluetooth_callback_handler(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {//is data package
		printf("Data Package received:\n");
		printf("[");
		for (int i=0; i<data_length; i++)
		{
			printf("%d:", i);
			if ((data_package[i] >= 40 && data_package[i] <=62) ||  (data_package[i] >= 65 && data_package[i] <=90) ||  (data_package[i] >= 97 && data_package[i] <=122))
				printf("%c", data_package[i]);
			else if (data_package[i] == 13)
				printf("<CR>\n");
			else if (data_package[i] == 10)
				printf("<LF>");
			else
				printf("[%d]",data_package[i]);
			printf(",");
			fflush(stdout);
		}
		printf("]\n");
	} else if (callback_type == 1) //connected
	{
		printf("Connected to: %s\n", data_package);
		connected = 1;
	} else if (callback_type == 2) //disconnected
	{
		printf("Disconnected from: %s\n", data_package);
		connected = 0;
	} else
		printf("Invalid callback type: %d\n", callback_type);
}


char buffer[255];
uint8_t cmd[255];


void master_test()
{
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

		int retval = bluetooth_cmd_set_remote_address((uint8_t*)"701A041CDBF1");

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


		/*for (int i=0; i<BLUETOOTH_DATA_PACKAGE_SIZE; i++)
		{
			bluetooth_data_package[i]=i+48;
		}*/

		if (bluetooth_send_data_package((uint8_t*)"Hallo", 5)==0)
					perror("Couldn't send cmd");

		if (retval ==1)
		{

			retval = bluetooth_cmd_online_command();
			printf("\n+++ ret=%d", retval);

			retval = bluetooth_cmd_close_connection();
			printf("\nATH ret=%d", retval);

		}


		printf("Gib daten ein:\n");

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
			/*if (strncmp(buffer, "+\n", 2)==0)
				count--;
			else
				cmd[count-1] = 13;*/

			//cmd[count] = 0;

			if (bluetooth_send_data_package(cmd, count-1)==0)
				perror("Couldn't send cmd");
		}
}

/**
 * Test if connection with bluetooth module is OK.
 * @param tries Number of tries
 * @return 1 on successful test, 0 otherwise
 */
uint8_t testConnection(uint8_t tries)
{
	uint8_t i;
	for (i=0; i<tries; i++)
	{
		if (bluetooth_cmd_test_connection()==1)
			break;
	}
	return (i<tries);
}

/**
 * Checks if it is already master (0).
 * If not it switches to master mode and disables autoconnect.
 * @return 1 on success, 0 on failure
 */
uint8_t setAsMaster()
{
	uint8_t* currentMode = bluetooth_cmd_get_mode();
	if (currentMode == NULL)
		return 0;
	if (currentMode[0] == '0') //already in master mode
		return 1;
	if (bluetooth_cmd_set_mode(0)==0)
		return 0;
	if (bluetooth_cmd_autoconnect(0)==0)
		return 0;
	else
		return 1;

}

/**
 * Checks if it is already slave (1).
 * If not it switches to slave mode and disables autoconnect.
 * @return 1 on success, 0 on failure
 */
uint8_t setAsSlave()
{
	uint8_t* currentMode = bluetooth_cmd_get_mode();
	if (currentMode == NULL)
		return 0;
	if (currentMode[0] == '1') //already in slave mode
		return 1;
	if (bluetooth_cmd_set_remote_address(NULL)==0)
		return 0;

	if (bluetooth_cmd_autoconnect(1)==0)
		return 0;
	if (bluetooth_cmd_set_mode(1)==0)
		return 0;
	else
		return 1;

}

void slave_test()
{
	if (setAsSlave() == 0)
	{
		printf("Couldn't set as slave\n");
		return;
	}

	printf("Waiting for connection ...\n");


	while (!connected)
	{
		usleep(1000);
	}

	printf("Gib daten ein:\n");

	while(connected)
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
		/*if (strncmp(buffer, "+\n", 2)==0)
			count--;
		else
			cmd[count-1] = 13;*/

		//cmd[count] = 0;

		if (bluetooth_send_data_package(cmd, count-1)==0)
			perror("Couldn't send cmd");
	}


}

int main()
{

	bluetooth_init(bluetooth_callback_handler);

	int ret = testConnection(4);
	if (ret == 0)
	{
		printf("Couldn't test connection!\n");
		return -1;
	}

	/*ret = setAsMaster();
	if (ret == 0)
	{
		printf("Couldn't set as master\n");
		return -1;
	}*/



	//return 0;

	/*int retval = bluetooth_cmd_set_pin((uint8_t*)"6666");
	printf("\nATP ret=%d",retval);

	uint8_t* ret = bluetooth_cmd_get_pin();
		printf("\nATP ret=%s",ret);

	//master_test();*/
	slave_test();

	bluetooth_close();
}

