#include <stdio.h>
#include "../protocols/downlink/downlink.h"
#include "../shared/common.h"

#define SERIAL
#define NUT

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
	bluetooth_init (downlink_bluetooth_callback_handler);
	int result = bluetooth_test_connection (4);
	assert (result == 1, "Could not test the connection");
	int result = bluetooth_set_as_slave ();
	assert (result == 1, "Could not set slave mode");

	bluetooth_close();
}

