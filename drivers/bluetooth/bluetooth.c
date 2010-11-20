/**
 * bluetooth.c - source for the Bluetooth driver
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 * \author Linus Lotz<lotz@in.tum.de>
 * \author Salomon Sickert
 */

#include "bluetooth.h"
#include <uart/uart.h>
#include <fifo.h>
#include <string.h>
#include <util/delay.h>

/**
 * Baudrate for the UART-connection to the BTM-222 on SQUIRREL
 */
#ifdef SQUIRREL
#define UART_BAUD_RATE      115200
#endif

#ifdef NUT
#define UART_BAUD_RATE      19200
#endif

typedef enum
{
    BT_RAW,
    BT_DATA,
    BT_CMD
} communication_mode_t;

#define BT_CMD_TIMEOUT_MS 1000

typedef enum
{
    BT_TEST,                      // AT
    BT_CONNECT,                   // ATA
    BT_DISCONNECT,                // ATH
    BT_CLEAR_ADDRESS,             // ATD0
    BT_SET_ADDRESS,               // ATD=_____
    BT_FIND_DEVICES,              // ATF?
    BT_DISABLE_AUTOCONNECT,       // ATO1
    BT_SET_MASTER,                // ATR0
    BT_SET_SLAVE,                 // ATR1
} bt_cmd_t;

#ifdef SQUIRREL
#define IN_FIFO_SIZE 512
#endif

#ifdef NUT
#define IN_FIFO_SIZE 65
#endif

static uint8_t bt_buffer[IN_FIFO_SIZE];
static fifo_t in_fifo;

static bt_mode_t bt_mode = BLUETOOTH_SLAVE;
static communication_mode_t comm_mode = BT_CMD;

static void
uart_receive (void)
{
    unsigned int uart_data;

    while (!fifo_is_full (&in_fifo))
    {
        uart_data = uart_getc ();

        switch (uart_data & 0xFF00)
        {
            /* Framing Error detected, i.e no stop bit detected */
        case UART_FRAME_ERROR:
            warn_pgm (PSTR ("FRM ERR"));
            return;

            /*
             * Overrun, a character already presend in the UART UDR register was
             * not read by the interrupt handler before the next character arrived,
             * one or more received characters have been dropped
             */
        case UART_OVERRUN_ERROR:
            warn_pgm (PSTR ("OVR ERR"));
            return;

            /*
             * We are not reading the receive buffer fast enough,
             * one or more received character have been dropped
             */
        case UART_BUFFER_OVERFLOW:
            warn_pgm (PSTR ("BUF ERR"));
            return;

            /* UART Inputbuffer empty, nothing to do */
        case UART_NO_DATA:
            return;

        default:
            fifo_write (&in_fifo, uart_data);
        }
    }

    warn_pgm (PSTR ("FIFO OVR ERR"));
}

static void
uart_send (const char *data, const uint8_t length)
{
    char echo;
    for (uint8_t i = 0; i < length; i++)
    {
        if (uart_putc (data[i]) == 0)
        {
            warn_pgm (PSTR ("UART: Remote not ready"));
            return;
        }

        if (comm_mode == BT_RAW)
            _delay_ms (50);

        /* Check for echo */
        if (comm_mode == BT_CMD)
        {
            while_timeout (fifo_is_empty(&in_fifo), 100)
            uart_receive();

            fifo_read (&in_fifo, &echo);

            if (echo != data[i])
                error_pgm (PSTR ("BT: WRONG ECHO"));
        }
    }
}


void test (void);

static bool
send_cmd (const bt_cmd_t command, const char *data)
{
    _delay_ms(500);
    // Check for command mode?
    char full_command[20];        // Maximum command size

    switch (command)
    {
    case BT_TEST:
        strcpy_P (full_command, PSTR ("AT"));
        break;

    case BT_CONNECT:
        strcpy_P (full_command, PSTR ("ATA"));
        break;

    case BT_DISCONNECT:
        strcpy_P (full_command, PSTR ("ATH"));
        break;

    case BT_CLEAR_ADDRESS:
        strcpy_P (full_command, PSTR ("ATD0"));
        break;

    case BT_SET_ADDRESS:
        strcpy_P (full_command, PSTR ("ATD="));
        memcpy((full_command+strlen(full_command)), data, 12);
        full_command[16] = 0;
        break;

    case BT_FIND_DEVICES:
        strcpy_P (full_command, PSTR ("ATF?"));
        break;

    case BT_DISABLE_AUTOCONNECT:
        strcpy_P (full_command, PSTR ("ATO1"));
        break;

    case BT_SET_MASTER:
        strcpy_P (full_command, PSTR ("ATR0"));
        break;

    case BT_SET_SLAVE:
        strcpy_P (full_command, PSTR ("ATR1"));
        break;

    default:
        warn_pgm (PSTR ("CMD UNK"));
        return false;
    }

    strcat_P (full_command, PSTR ("\r"));

    // throw away your television
    uart_receive ();
    fifo_clear (&in_fifo);

    // send command
    uart_send (full_command, strlen (full_command));
    // get response
    while_timeout (true, BT_CMD_TIMEOUT_MS)
    {
        uart_receive ();
        if (fifo_strstr_pgm (&in_fifo, PSTR ("OK\r\n")))
            return true;
        if (fifo_strstr_pgm (&in_fifo, PSTR ("ERROR\r\n")))
            return false;
    }

    if (command != BT_TEST)
        warn_pgm (PSTR ("CMD SEND: TIMEOUT"));

    return false;
}

void test ()
{
    comm_mode = BT_RAW;
    for (uint8_t i = 0; i < 20; i++)
        if (send_cmd (BT_TEST, NULL))
            break;
    comm_mode = BT_CMD;
}

static void
clean_line (void)
{
    while_timeout(fifo_strstr_pgm (&in_fifo, PSTR ("\r\n")),1000)
    uart_receive();
}

static communication_mode_t
update_comm_mode (uint16_t timeout_ms)
{
    while_timeout(true, timeout_ms)
    {
        uart_receive ();

        if (fifo_strstr_pgm (&in_fifo, PSTR ("DISCONNECT")))
        {
            clean_line ();
            return comm_mode = BT_CMD;
        }

        if (fifo_strstr_pgm (&in_fifo, PSTR ("CONNECT")))
        {
            clean_line ();
            debug_pgm(PSTR("CONNECTED"));
            return comm_mode = BT_DATA;
        }

        if (fifo_strstr_pgm (&in_fifo, PSTR ("Time out,Fail to connect!")))
        {
            clean_line ();
            return comm_mode = BT_CMD;
        }
    }

    return comm_mode;
}

bool
bt_init (void)
{
    uart_init (UART_BAUD_SELECT (UART_BAUD_RATE, F_CPU));
    fifo_init (&in_fifo, bt_buffer, IN_FIFO_SIZE);

    // Enable ECHO
    comm_mode = BT_RAW;

    _delay_ms (2000);
    //uart_send ("ATZ0\r", 5);


    //_delay_ms (50);
    // throw away your television
    uart_receive ();
    fifo_clear (&in_fifo);

    comm_mode = BT_CMD;

    test();

    send_cmd (BT_CLEAR_ADDRESS, NULL);
    test();
    send_cmd (BT_SET_SLAVE, NULL);
    test();
    return true;
}


bool
bt_set_mode (const bt_mode_t mode)
{
    if (update_comm_mode(0) == BT_DATA)
        return false;

    if (mode == bt_mode)
        return true;

    if (mode == BLUETOOTH_MASTER)
        if (send_cmd (BT_SET_MASTER, NULL))
        {
            bt_mode = BLUETOOTH_MASTER;
            test();
            send_cmd (BT_DISABLE_AUTOCONNECT, NULL);
        }

    if (mode == BLUETOOTH_SLAVE)
        if (send_cmd (BT_SET_SLAVE, NULL))
        {
            bt_mode = BLUETOOTH_SLAVE;
        }

    test();
    return mode == bt_mode;
}

bool
bt_receive (void * data, uint8_t * length, uint16_t timeout_ms)
{
    while_timeout(true, timeout_ms)
    {
        uint8_t receive_length;

        uart_receive ();

        if (fifo_is_empty (&in_fifo))
            continue;

        if (update_comm_mode (0) == BT_CMD)
        {
            debug_pgm (PSTR ("not connected"));
            continue;
        }

        fifo_read (&in_fifo, (char *) &receive_length);

        if (receive_length > *length)
        {
            byte_to_hex (receive_length);
            byte_to_hex (*length);
            debug_pgm (PSTR ("receive_length > length"));
            return false;
        }

        *length = receive_length;

        // Warning!! if no data arrives this function will be stuck
        for (uint8_t i = 0; i < receive_length; i++)
        {
            if (update_comm_mode(0) == BT_CMD)
            {
                debug_pgm (PSTR ("not connected"));
                return false;
            }

            while (fifo_is_empty (&in_fifo))
                uart_receive ();

            fifo_read (&in_fifo, (char *) data + i);
        }
        return true;
    }
    return false;
}


bool
bt_send (void *data, const uint8_t length)
{
    if (update_comm_mode (0) == BT_CMD)
        return false;

    uart_send ( (const char *) &length, 1);

    uart_send ( (char *) data, length);

    return (update_comm_mode (0) == BT_DATA);
}

#ifdef SQUIRREL
bool
bt_connect (const char *address)
{
    debug_pgm (PSTR ("CONNECT"));
    test();
    if (!send_cmd (BT_SET_ADDRESS, address))
        return false;

    test();
    if (!send_cmd (BT_CONNECT, NULL))
        return false;

    debug_pgm (PSTR ("WAIT FOR COMM"));

    return (BT_DATA == update_comm_mode (30000));
}

bool
bt_disconnect (void)
{
    // Switch to online cmd mode
    for (uint8_t i = 0; i < 4; i++)
    {
        const char plus = '+';
        uart_send (&plus, 1);
        _delay_ms (1500);
    }

    comm_mode = BT_CMD;

    if (!send_cmd (BT_DISCONNECT, NULL))
        return false;

    return (BT_CMD == update_comm_mode (60000));
}

void
copy_address (const char *src, char *dst)
{
    uint8_t off = 0;

    for (uint8_t i = 0; i < 14; i++)
    {
        if (src[i+off] == '-')
            off++;

        dst[i] = src[i+off];
    }
}

bool
bt_discover (char result[8][12], void (*update_callback)(const uint8_t progress))
{
    if (!bt_set_mode(BLUETOOTH_MASTER)) return false;
    if (!send_cmd (BT_FIND_DEVICES, NULL)) return false;

    char buffer[255]; //oversized, but who cares?
    char *bufferhead = buffer;
    uint8_t pos = 0;

    while_timeout (!fifo_strstr_pgm (&in_fifo, PSTR ("Inquiry Results:\r\n")), 12000)
    uart_receive();

    for (uint16_t i = 0; i < 60000; i++)
    {
        uart_receive();

        if ((i % 1000) == 0) //&& update_callback != NULL)
        {
            error_putc('.');
        }

        _delay_ms (1);
    }

    while (!fifo_is_empty (&in_fifo))
    {
        // Get next line
        while (!fifo_cmp_pgm (&in_fifo, PSTR ("\r\n")))
        {
            fifo_read (&in_fifo, bufferhead);
            bufferhead++;
        }
        //terminate string
        *bufferhead = 0;

        //reset bufferhead
        bufferhead = buffer;

        if (strlen (buffer) == 0)
            continue; //the empty line before end of inquiry

        if (strstr_P (buffer, PSTR ("Inquiry End")))
        {
            error_putc('0'+pos);
            fifo_clear (&in_fifo);
            test();
            return true;
        }

        copy_address (&buffer[21], result[pos]);
        pos++;
    }
    test();

    return false;
}

#endif /* SQUIRREL */
