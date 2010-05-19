/*
 * squirrel.c
 *
 *  Created on: May 12, 2010
 *      Author: seanlabastille
 */
#include "squirrel.h"
#include "../drivers/bluetooth/bluetooth.h"
#ifdef SERIAL
#include <stdio.h>
#endif

	// Proposed lookup arrays. Every Nut receives a running number when first encountered
	// I think an unbounded array is a better idea, as we don't need to store these huge bunch of pointers. √
	// These arrays are indexed as follows: [running number]([actor/sensor/configuration ID])
	typedef struct device_info {
		char mac[6];
		uint8_t class;
		uint8_t actor_types[16];
		uint8_t sensor_types[16];
		uint8_t config_types[16];
	} device_info;
	struct device_info* device_list[16]; // FIXME: Unbounded array
#ifndef SERIAL
	int main(void) {
		squirrel_init();
		return 0;
	}
#endif

	/**
	 * Test if connection with bluetooth module is OK.
	 * @param tries Number of tries
	 * @return 1 on successful test, 0 otherwise
	 */
	uint8_t bluetooth_test_connection(uint8_t tries)
	{
		uint8_t i;
		for (i=0; i<tries; i++)
		{
			if (bluetooth_cmd_test_connection()==1)
				break;
		}
		return (i<tries);
	}


	void bluetooth_handle(uint8_t *data_package, const uint8_t callback_type, const uint8_t data_length) {
#ifdef SERIAL
		printf("Handler called: %d", callback_type);
		if (callback_type == 0) {//is data package
				printf("Data Package received:\n");
				if (data_length == 64) {
										printf("Uplink Packet.");
										uplink_handle_package(data_package);
									} else if (data_length == 4) {
										printf("Downlink Packet.");
										downlink_handle_package(data_package);
									}
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
#endif
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

			int retval = bluetooth_cmd_set_remote_address((uint8_t*)"701A041CDBF1");
#ifdef SERIAL
			printf("\nATD ret=%d",retval);
#endif

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
#ifdef SERIAL
			printf("\nATA ret=%d", retval);
#endif

			if (retval == 0)
			{
				retval = bluetooth_cmd_connect(1);
#ifdef SERIAL
				printf("\nATA ret=%d", retval);
#endif
			}


			/*for (int i=0; i<BLUETOOTH_DATA_PACKAGE_SIZE; i++)
			{
				bluetooth_data_package[i]=i+48;
			}*/

			if (bluetooth_send_data_package((uint8_t*)"Hallo", 5)==0) {
#ifdef SERIAL
						perror("Couldn't send cmd");
#endif
			}

			if (retval ==1)
			{

				retval = bluetooth_cmd_online_command();
#ifdef SERIAL
				printf("\n+++ ret=%d", retval);
#endif
				retval = bluetooth_cmd_close_connection();
#ifdef SERIAL
				printf("\nATH ret=%d", retval);
#endif

			}

#ifdef SERIAL
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
#endif
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
#ifdef SERIAL
			printf("Couldn't set as slave\n");
#endif
			return;
		}
#ifdef SERIAL
		printf("Waiting for connection ...\n");
#endif

		while (!connected)
		{
			usleep(1000);
		}
#ifdef SERIAL
		printf("Gib daten ein:\n");
#endif
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

			if (bluetooth_send_data_package(cmd, count-1)==0) {
#ifdef SERIAL
				perror("Couldn't send cmd");
#endif
			}
		}


	}

	void squirrel_main() {

	}

	void squirrel_init(void) {
		bluetooth_init(bluetooth_handle);
		int ret = bluetooth_test_connection(4);
		if (ret) {
			//slave_test();
			master_test();
			squirrel_main();
		} else {
#ifdef SERIAL
			printf("Bluetooth test failed");
#endif
		}

	}

