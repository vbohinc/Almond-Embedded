/*
* @file bluetooth_serial_test.c
* @author Stefan Profanter
* @date 06.05.2010
*
* This file is used on the PC to send commands over the /dev/ttysUSB terminal to the BTM-222 Module.
* It's purpose is to test command handling. So don't compile this file for embedded system.
*
*/
#include "./drivers/bluetooth/bluetooth.h"

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int connected = 0;

uint32_t error_count = 0;
uint32_t send_count = 0;


uint8_t int_received = 0;
uint8_t int_to_send = 1;

uint8_t pkg_received = 0;



char buffer[255];
uint8_t cmd[255];

void bluetooth_callback_handler(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {//is data package
		for (int i=0; i<data_length; i++)
		{
			/*if (data_package[i] > int_received+1)
			{
				printf("ERROR: Int: %d, ErrCnt: %d, SendCnt:%d, Rate:%f\n", int_received, error_count, send_count-data_length+i, (float)error_count/(float)send_count);
				error_count++;
			}*/
			int_received = data_package[i];
		}
		pkg_received = 1;
		//printf("]\n");
	} else if (callback_type == 1) //connected
	{
		bluetooth_array_to_address(data_package, cmd, 0,0,0);
		printf("Connected to: %s\n", buffer);
		connected = 1;
	} else if (callback_type == 2) //disconnected
	{
		bluetooth_array_to_address(data_package, cmd, 0,0,0);
		printf("Disconnected from: %s\n", buffer);
		connected = 0;
	} else
		printf("Invalid callback type: %d\n", callback_type);
}


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

		if (bluetooth_set_as_master() == 0)
		{
			printf("Couldn't set as master\n");
			return;
		}

		char addr[12] = "701A041CDBF1";
		uint8_t compressed[6];
		bluetooth_address_to_array((uint8_t*)addr, compressed, 0,0,0);

		int retval = bluetooth_cmd_set_remote_address(compressed);

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

		//if (bluetooth_send_data_package((uint8_t*)"Hallo", 5)==0)
		//			perror("Couldn't send cmd");

		/*if (retval ==1)
		{

			retval = bluetooth_cmd_online_command();
			printf("\n+++ ret=%d", retval);

			retval = bluetooth_cmd_close_connection();
			printf("\nATH ret=%d", retval);

		}*/


		printf("Gib daten ein:\n");

		uint32_t sentPackages = 0;

		uint8_t package_length = 64;

		for (int i=0; i<package_length; i++)
		{
			cmd[i] = int_to_send;
			int_to_send++;
			send_count++;
		}

		time_t start, finish;
		start = time(NULL);
		uint32_t sent_bytes = 0;

		while(1)
		{
			//bluetooth_process_data();



			int ret = bluetooth_send_data_package(cmd, &package_length, 0, 2000);
			if (ret != 0)
			{
				printf("Send package returned: %d\n", ret);
				return;
			}
			sentPackages++;
			sent_bytes+=(package_length);
			//bluetooth_process_data();

			//if (sentPackages%10== 0)
			{

				finish = time(NULL) - start;
				double time = (double)(finish);
				printf("SentPackages: %d, Bytes:%d, Time:%ds Speed:%d B/s\n", sentPackages, sent_bytes,finish,  (int)(((float)sent_bytes/time)+0.5));
			}

			/*fgets(buffer, 255, stdin);
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
				cmd[count-1] = 13;*/

			//cmd[count] = 0;

			/*if (bluetooth_send_data_package(cmd, count-1)==0)
				perror("Couldn't send cmd");
			bluetooth_process_data();*/
		}
}

void slave_test()
{
	if (bluetooth_set_as_slave() == 0)
	{
		printf("Couldn't set as slave\n");
		return;
	}

	printf("Waiting for connection ...\n");


	while (!connected)
	{
		bluetooth_process_data();
		usleep(1000);
	}

	printf("Gib daten ein:\n");

	while(connected)
	{
		bluetooth_process_data();
		fgets(buffer, 255, stdin);
		if (strncmp(buffer, "exit",4)==0)
			break;
		uint8_t count = 0;
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

		count--;

		if (bluetooth_send_data_package(cmd, &count, 0, 0)!=0)
			perror("Couldn't send cmd");
		bluetooth_process_data();
	}


}

int main()
{
	bluetooth_init(bluetooth_callback_handler);

	int ret = bluetooth_test_connection(4);
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
 */
	master_test();

	//slave_test();

	bluetooth_close();
}

