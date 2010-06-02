#include <stdio.h>
#include "/home/seanlabastille/workspace/AlmondSquirrelTest/protocols/downlink/downlink.h"
#include "../shared/common.h"
#include "/home/seanlabastille/workspace/AlmondSquirrelTest/bluetoothbackend/bluetooth_server.h"
#include "/home/seanlabastille/workspace/AlmondSquirrelTest/bluetoothbackend/bluetooth_client.h"
#include <string.h>
#include <unistd.h>
#include <time.h>

#define SQUIRREL

void server_data_callback(const uint8_t *data_package, const uint8_t data_length)
{

	/*for (int i=0; i<data_length; i++)
	{
		if (data_package[i] > int_received+1)
		{
			printf("ERROR: Int: %d, ErrCnt: %d, SendCnt:%d, Rate:%f\n", int_received, error_count, received_count-data_length+i, (float)error_count/(float)received_count);
			error_count++;
		}
		int_received = data_package[i];
		received_count++;
	}*/

	//usleep(10000000);

	//send data back to client
	bluetooth_server_send_data(data_package, data_length);
	//return 0;
}

int client_example()
{
	/*******************************
		 * Client example
		 *******************************/
		printf("Connecting to client ...\n");
		int ret = bluetooth_client_connect("00:12:6F:08:59:F9", server_data_callback);

		if (ret != 1)
		{
			perror("Couldn't connect to client");
			return 0;
		}

		uint8_t buffer[255];

		while(1)
		{
			bool *err;
			err = false;
			printf("%d", downlink_get_sensor_class(1, err));
			printf("\nWrite new data:");
			if (fgets((char*)buffer, 255, stdin)==NULL)
				perror("Couldn't read new command");
			if (strncmp((char*)buffer, "exit",4)==0)
				break;
			int count = 0;
			while(buffer[count]!=0)
			{
				count++;
			}

			int ret = bluetooth_client_send_data(buffer, count-1);


			if (ret<0)
				perror("Couldn't send cmd");
		}

		return 0;
}

int main(int argc, char **argv)
{
	//return server_example();

	return client_example();

}

/*
int server_example() {

	/*******************************
	 * Server example
	 *******************************
	bluetooth_server_start(server_data_callback, server_error_callback);

	printf("Waiting for client to accept\n");
	//wait until client is connected
	while (bluetooth_server_connected_with[0] == 0)
	{
		usleep(100000);
	}

	uint8_t buffer[255];

	while(bluetooth_server_connected_with[0] != 0)
	{
		printf("\nNew cmd:");
		if (fgets((char*)buffer, 255, stdin)==NULL)
			perror("Couldn't read new command");
		if (strncmp((char*)buffer, "exit",4)==0)
			break;
		int count = 0;
		while(buffer[count]!=0)
		{
			count++;
		}

		int ret = bluetooth_server_send_data(buffer, count-1);


		if (ret<0)
			perror("Couldn't send cmd");
	}

	return 0;

}*/

