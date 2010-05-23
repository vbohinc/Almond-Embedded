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
#include <string.h>
#include <time.h>
#include <unistd.h>
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


	void bluetooth_handle(uint8_t * data_package, const uint8_t callback_type, const uint8_t data_length) {
#ifdef SERIAL
		printf("Handler called: %d", callback_type);
		if (callback_type == 0) {//is data package
				printf("Data Package received:\n");
				if (data_length == 64) {
										printf("Uplink Packet.");
										uplink_handle_packet((struct uplink_packet *) data_package);
									} else if (data_length == 4) {
										printf("Downlink Packet.");
										downlink_handle_packet((struct downlink_packet *) data_package);
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
				// bluetooth_is_connected is set
			} else if (callback_type == 2) //disconnected
			{
				printf("Disconnected from: %s\n", data_package);
				// bluetooth_is_connected is set
			} else
				printf("Invalid callback type: %d\n", callback_type);
#endif
		}


	void master_test(void)
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

			setAsMaster();

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

#ifdef SERIAL
			printf("Gib daten ein:\n");
			char buffer[255];
			uint8_t cmd[255];
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
					cmd[count-1] = 13;

				cmd[count] = 0;*/

				if (bluetooth_send_data_package(cmd, count-1)==0)
					perror("Couldn't send cmd");
			}
#endif
	}


	void slave_test(void)
	{
#ifdef SERIAL
		if (setAsSlave() == 0)
		{
			printf("Couldn't set as slave\n");

			return;
		}

		printf("Waiting for connection ...\n");


		while (!bluetooth_is_connected)
		{
			usleep(1000);
		}

		printf("Gib daten ein:\n");

		while(bluetooth_is_connected)
		{
			char buffer[255];
			uint8_t cmd[255];
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
				perror("Couldn't send cmd");

			}
		}
#endif

	}

	void squirrel_main(void) {
#ifdef SERIAL
			printf("Squirrel Main");
#endif

	}

	void squirrel_init(void) {
		bluetooth_init(bluetooth_handle);
		int ret = bluetooth_test_connection(4);
		if (ret) {
			//if (bluetooth_cmd_get_mode()[0] == '0') {
			//	printf("Bluetooth in Master Mode - Switching to Slave");
			//	slave_test();
			//} else {
			//	printf("Bluetooth in Slave Mode - Switching to Master");
				master_test();
			//}
			squirrel_main();
		} else {
#ifdef SERIAL
			printf("Bluetooth test failed");
#endif
		}

	}

