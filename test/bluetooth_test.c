/*
 * main.c
 *
 *  Created on: 02.05.2010
 *      Author: stefan
 */

#ifdef __AVR_ATxmega128A1__
#define LED1_DDR PORTD.DIR
#define LED1_PORT PORTD.OUT
#define LED2_DDR PORTD.DIR
#define LED2_PORT PORTD.OUT
#define LED1_PIN PIN3
#define LED2_PIN PIN6
#else
#define LED1_DDR DDRD
#define LED1_PORT PORTD
#define LED2_DDR DDRC
#define LED2_PORT PORTC
#define LED1_PIN 7
#define LED2_PIN 0
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "bluetooth/bluetooth.h"
#include "../shared/error.h"

int connected = 0;

//char buffer[10];
uint8_t cmd[20];
uint32_t error_count = 0;
uint32_t send_count = 0;

uint8_t int_to_send = 1;

uint8_t pkg_received = 0;

uint8_t return_arr[4];

void bluetooth_callback_handler(char *data_package, const uint8_t callback_type, const uint8_t data_length)
{
	if (callback_type == 0) {//is data package


		debug_pgm(PSTR("Package:"));
		for (int i=0; i<data_length; i++)
		{
			/*if (data_package[i] > int_received+1)
			{
				printf("ERROR: Int: %d, ErrCnt: %d, SendCnt:%d, Rate:%f\n", int_received, error_count, send_count-data_length+i, (float)error_count/(float)send_count);
				error_count++;
			}*/

			error_putc((char)data_package[i]);

		}
		pkg_received = 1;
		//printf("]\n");
	} else if (callback_type == 1) //connected
	{
		bluetooth_array_to_address((char*)data_package, (char*)cmd, 0);
		debug_pgm(PSTR("Connected:"));
		debug((char*)cmd);
		error_putc('\n');
		connected = 1;
	} else if (callback_type == 2) //disconnected
	{
		bluetooth_array_to_address((char*)data_package, (char*)cmd, 0);
		debug_pgm(PSTR("Disconnected:"));
		debug((char*)cmd);
		error_putc('\n');
		connected = 0;
	}
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

		if (bluetooth_set_as_master() == 0)
		{
			error_pgm(PSTR("BTM:Couldn't set as master\n"));
			//printf("Couldn't set as master\n");
			return;
		}

		cmd[0] = '7';
		cmd[1] = '0';
		cmd[2] = '1';
		cmd[3] = 'A';
		cmd[4] = '0';
		cmd[5] = '4';
		cmd[6] = '1';
		cmd[7] = 'C';
		cmd[8] = 'D';
		cmd[9] = 'B';
		cmd[10] = 'F';
		cmd[11] = '1';

		uint8_t compressed[6];
		bluetooth_address_to_array((char*)cmd, (char*)compressed, 0);

		//printf("\nATD ret=%d",retval);

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

		bluetooth_cmd_connect(0);

	//	bluetooth_putc(BLUETOOTH_SPECIAL_BYTE);
	//	bluetooth_putc(BLUETOOTH_STOP_BYTE);

		uint8_t pkg[128];
		for (uint8_t x=0; x<64; x++)
					pkg[x] = x;
		uint8_t len;


		while (1)
		{
			len = 64;
			bluetooth_send_data_package_with_response(pkg, &len, 2000);
			//uint8_t ret =
			//if (ret == 1)
			//	print((uint8_t*)"UE\n");
			//else if (ret == 2)
				//print((uint8_t*)"TO\n");

			//print((uint8_t*)"RLn:");
			//FTDISend(len+48);
			//FTDISend(10);
			if (len != 64)
			{
				//print((uint8_t*)"ERL:");
				//FTDISend(len+48);
				continue;
			}
			uint8_t x;
			for (x=0; x<64; x++)
			{
				if (pkg[x] != x)
				{
					//print((uint8_t*)"ER:");
					//FTDISend(x+48);
					break;
				}
			}
			//if (x>=i)
				//print((uint8_t*)"K");
		}
}

void slave_test(void)
{



	if (bluetooth_cmd_discoverable(1) == 0)
	{
		error_pgm(PSTR("BTM:Couldn't set discoverable\n"));
		return;
	}


	if (bluetooth_set_as_slave() == 0)
	{
		error_pgm(PSTR("BTM:Couldn't set as slave\n"));
		return;
	}

	debug_pgm(PSTR("Waiting for connection ...\n"));


	while (!connected)
	{
		bluetooth_process_data();
	}

	//printf("Gib daten ein:\n");

	while(connected)
	{
		bluetooth_process_data();
		//fgets(buffer, 255, stdin);
		//if (strncmp(buffer, "exit",4)==0)
		//	break;
		/*uint8_t count = 0;
		while(buffer[count]!=0)
		{
			cmd[count] = buffer[count];
			count++;
		}*/
		/*if (strncmp(buffer, "+\n", 2)==0)
			count--;
		else
			cmd[count-1] = 13;*/

		//cmd[count] = 0;

		//count--;

		//if (bluetooth_send_data_package(cmd, &count, 0, 0)!=0)
		//	perror("Couldn't send cmd");
		//bluetooth_process_data();
	}


}

//uint8_t arr[200] EEMEM = " asdfasdfasdfjasdflkjasdfhlkjasfhlkjsajgaljfaklhfaklaskljfasdfasdfasdfjasdflkjasdfhlkjasfhlkjsajgaljfaklhfaklaskljfasdfasdfasdfjasdflkjasdfhlkjasfhlkjsajgaljfaklhfaklaskljf";



int main(void)
{
	LED1_DDR |= 0xFF;

	LED2_DDR |= 0xFF;


	LED1_PORT |= (1<<LED1_PIN);
	LED2_PORT |= (1<<LED2_PIN);
	_delay_ms(500);
	LED1_PORT &= ~(1<<LED1_PIN);
	LED2_PORT &= ~(1<<LED2_PIN);
	_delay_ms(500);
	error_init();
	bluetooth_init(bluetooth_callback_handler);
	sei();

	debug_pgm(PSTR("### Bluetooth Test ###"));

	LED1_PORT |= (1<<LED1_PIN);
	LED2_PORT |= (1<<LED2_PIN);
	_delay_ms(500);
	LED1_PORT &= ~(1<<LED1_PIN);
	LED2_PORT &= ~(1<<LED2_PIN);
	_delay_ms(500);


/*	uint8_t buf[10];
	buf[0] = 'A';
	buf[1] = 'T';
	buf[2] = 'L';
	buf[3]='2';
	buf[4] = 13; //<CR>
	buf[5] = 0;
	bluetooth_cmd_send(buf,BLUETOOTH_CMD_WAIT_TIME);*/


	debug_pgm(PSTR("BTM: Testing Connection"));
	int ret = bluetooth_test_connection(4);

	if (ret == 0)
	{

		error_pgm(PSTR("BTM: Test Conn=ERROR"));
		LED1_PORT |= (1<<LED1_PIN);
	}
	else
	{
		debug_pgm(PSTR("BTM: Test Conn=OK"));
		LED2_PORT |= (1<<LED2_PIN);
		char* addr = bluetooth_cmd_get_address();
		char full[13];
		bluetooth_array_to_address(addr, full, 0);
		full[12] = 0;
		debug_pgm(PSTR("BTM: My Address:"));
		debug(full);
		//master_test();
		slave_test();
	}
}

