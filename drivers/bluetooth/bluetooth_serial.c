/**
* @file bluetooth_serial.c
* @author Stefan Profanter
* @date 02.05.2010
*
* This file contains functions for test purpose on the pc. It replaces the function of the USART on the
* microcontroller and is used, if the BTM-222 is connected over USB to the PC
*
*/

#include "bluetooth_serial.h"
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>



int bluetooth_serial_port_fd = 0;
int bluetooth_serial_thread_abort = 0;

pthread_t bluetooth_serial_read_thread;

void *bluetooth_serial_thread_read()
{
	/* read characters into our string buffer until we get a CR or NL */
	uint8_t buffer[20];
	while (!bluetooth_serial_thread_abort)
	{
		uint8_t ret = read(bluetooth_serial_port_fd, buffer, 20);
		if (ret > 0)
		{
			for (int i=0; i<ret; i++)
				bluetooth_serial_byte_received(buffer[i]);
		}
		else
		{
		    struct timespec s;
		    s.tv_sec = 0;
		    s.tv_nsec = 100000L;
		    nanosleep(&s, NULL);

		}
	}
	return NULL;

}


void bluetooth_serial_init(void)
{
	struct termios options;

	/* open the port */
	bluetooth_serial_port_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

	if (bluetooth_serial_port_fd==-1)
		perror("Couldn't open serial port:");

	fcntl(bluetooth_serial_port_fd, F_SETFL, 0);

	/* get the current options */
	tcgetattr(bluetooth_serial_port_fd, &options);

	/*
	 * Set the baud rates to 19200...
	 */

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);


	options.c_cflag &= ~CSIZE; /* Mask the character size bits */
	options.c_cflag |= CS8;    /* Select 8 data bits */


	/*
	 * Enable the receiver and set local mode...
	 */

	options.c_cflag |= (CLOCAL | CREAD);

	/* set raw input, 1 second timeout */
	options.c_lflag     &= ~(ICANON | ISIG);
	options.c_oflag     &= ~OPOST;
	options.c_cc[VMIN]  = 0;
	options.c_cc[VTIME] = 10;

	/* set the options */
	tcsetattr(bluetooth_serial_port_fd, TCSAFLUSH, &options);

	bluetooth_serial_thread_abort = 0;
	pthread_create (&bluetooth_serial_read_thread, NULL, bluetooth_serial_thread_read, NULL);


}


void bluetooth_serial_close(void)
{
	bluetooth_serial_thread_abort = 1;
	pthread_join (bluetooth_serial_read_thread, NULL);
	close(bluetooth_serial_port_fd);
}

void bluetooth_serial_set_byte_handler(void (*byte_handling_function)(uint8_t))
{
	bluetooth_serial_byte_handling_function = byte_handling_function;
}

uint8_t bluetooth_serial_send_bytes(uint8_t *bytes, const uint8_t length)
{

	for (uint8_t i=0; i<length; i++)
	{
		if (bluetooth_serial_putc(bytes[i])==0)
			return 0;
		//There must be a sleep otherwise the module returns always error
		struct timespec s;
		s.tv_sec = 0;
		//don't make it tooo small otherwise the module dosen't return correct values
		s.tv_nsec = 50000000L;
		nanosleep(&s, NULL);
	}

	return 1;
}


uint8_t bluetooth_serial_putc (const uint8_t byte)
{
	int  tries;        /* Number of tries so far */

	uint8_t bytes[1];
	bytes[0]=byte;

	for (tries = 0; tries < 3; tries ++)
	{
		/* send an AT command followed by a CR */
		if (write(bluetooth_serial_port_fd, bytes, 1) < 1)
		{
			perror("Couldn't write");
			continue;
		}
		return 1;
	}
	return 0;
}

void bluetooth_serial_byte_received(const uint8_t byte)
{
	bluetooth_serial_byte_handling_function(byte);
}
