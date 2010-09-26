#include "bluetooth.h"
#include <uart/uart.h>
#include <fifo.h>
#include <string.h>
#include <util/delay.h>

/**
 * Baudrate for the UART-connection to the BTM-222 on SQUIRREL
 */
#ifdef SQUIRREL
#define UART_BAUD_RATE      9600
#endif

#ifdef NUT
#define UART_BAUD_RATE      19200
#endif

typedef enum
{
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
  BT_SET_SLAVE                  // ATR1
#if 0
    TODO:BT_ENABLE_FLOWCONTROL BT_SET_BAUDRATE
#endif
} bt_cmd_t;

#ifdef SQUIRREL
#define IN_FIFO_SIZE 255
#endif

#ifdef NUT
#define IN_FIFO_SIZE 65
#endif

static uint8_t bt_buffer[IN_FIFO_SIZE];
static fifo_t in_fifo;

static bt_mode_t bt_mode = BLUETOOTH_SLAVE;
static communication_mode_t comm_mode = BT_CMD;

// Private helper

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

#ifdef DEBUG_BLUETOOTH
          error_putc (uart_data);
          if (uart_data == 10)
            error_putc ('#');
          if (uart_data == 13)
            error_putc ('+');
#endif

        }
    }

  warn_pgm (PSTR ("FIFO OVR ERR"));
}

static void
uart_send (const char *data, const uint8_t length)
{
  for (uint8_t i = 0; i < length; i++)
    {
      while (uart_putc (data[i]) == 0)
        {
          warn_pgm (PSTR ("UART: Remote not ready"));
          _delay_ms (1);
        }
    }
}

static bool
send_cmd (const bt_cmd_t command, const char *data)
{
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
      strcat (full_command, data);
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

  strcat_P (full_command, PSTR ("\r\n"));

  // throw away your television
  uart_receive ();
  fifo_clear (&in_fifo);

  // send command
  uart_send (full_command, strlen (full_command));

  // get response
  for (uint16_t i = 0; i < BT_CMD_TIMEOUT_MS; i++)
    {
      if(fifo_cmp_pgm(&in_fifo,PSTR("OK\r\n")))
      {
        return true;
      }
      if(fifo_cmp_pgm(&in_fifo,PSTR("ERROR\r\n")))
      {
        return false;
      }
      _delay_ms (1);
    }
  warn_pgm (PSTR ("CMD SEND: TIMEOUT"));
  return false;
}

static void
clean_line (void)
{
  for (uint16_t i = 0; i < 1000; i++)
    {
      if (fifo_strstr_pgm (&in_fifo, PSTR ("\r\n")))
        return;
        
      uart_receive ();
      _delay_ms (1);
    }
  warn_pgm ("MISSING \\r\\n");
}

static communication_mode_t
update_comm_mode (uint16_t timeout_ms)
{
  for (uint16_t i = 0; i < timeout_ms; i++)
    {
      uart_receive ();

      if (fifo_cmp_pgm (&in_fifo, PSTR ("DISCONNECT")))
        {
          clean_line ();
          return comm_mode = BT_CMD;
        }

      if (fifo_cmp_pgm (&in_fifo, PSTR ("CONNECT")))
        {
          clean_line ();
          return comm_mode = BT_DATA;
        }

      if (fifo_cmp_pgm (&in_fifo, PSTR ("Time out,Fail to connect!")))
        {
          clean_line ();
          return comm_mode = BT_CMD;
        }

      _delay_ms (1);
    }

  if(timeout_ms != 1)
    warn_pgm (PSTR ("Received no answer!"));
  return comm_mode;
}

static bool
check_package (uint8_t * data, uint8_t * length)
{
  uart_receive ();

  if (fifo_is_empty (&in_fifo))
    return false;

  if (update_comm_mode(1) == BT_CMD)
    return false;

  // char?
  fifo_read (&in_fifo, (char *) length);

  // Warning!! if no data arrives this function will be stuck
  for (uint8_t i = 0; i < *length; i++)
    {
      while (fifo_is_empty (&in_fifo))
        uart_receive ();

      if (update_comm_mode(1) == BT_CMD)
        return false;

      fifo_read (&in_fifo, (char *) &data[i]);

    }
  return true;
}

bool
bt_init (void)
{
  uart_init (UART_BAUD_SELECT (UART_BAUD_RATE, F_CPU));

  //init fifos
  fifo_init (&in_fifo, bt_buffer, IN_FIFO_SIZE);

  bool connection = false;
  for (int i = 0; i < 5; i++)
    {
      if (send_cmd (BT_TEST, NULL))
        {
          connection = true;
          break;
        }
    }
  if (!connection)
    {
      error_pgm ("BT: Init failed");
      return false;
    }

  if (!send_cmd (BT_CLEAR_ADDRESS, NULL))
    return false;

  send_cmd (BT_DISABLE_AUTOCONNECT, NULL);      //fails in slave so no test

  return true;
}

bool
bt_set_mode (const bt_mode_t mode)
{


  // Set bt_set_master... kill autoconnect
  if (!send_cmd (BT_SET_MASTER, NULL))
    return false;
  if (mode == BLUETOOTH_MASTER)
    {
      if (send_cmd (BT_SET_MASTER, NULL))
        {
          bt_mode = mode;
          return true;
        }
      else
        return false;
    }
  else if (mode == BLUETOOTH_SLAVE)
    {
      if (send_cmd (BT_SET_SLAVE, NULL))
        {
          bt_mode = mode;
          return true;
        }
      else
        return false;
    }
  else
    return false;
}

bool
bt_receive (uint8_t * data, uint8_t * length)
{
  if (comm_mode == BT_CMD)
    {
      if (update_comm_mode(1) == BT_CMD)
        {
          return false;
        }
    }
  //comm_mode == BT_DATA 
  return check_package (data, length);
}


bool
bt_send (const char *data, const uint8_t length)
{
  uart_send ((const char *) &length, 1);
  uart_send (data, length);
  return (update_comm_mode (1) == BT_DATA);
}

#ifdef SQUIRREL
bool
bt_connect (const char *address)
{
  if (!send_cmd (BT_SET_ADDRESS, address))
    return false;

  if (!send_cmd (BT_CONNECT, NULL))
    return false;

  return (BT_DATA == update_comm_mode (60000));
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

  if (!send_cmd (BT_DISCONNECT, NULL))
    return false;

  return (BT_CMD == update_comm_mode (60000));
}


bool
bt_discover (const char *result)
{
  return false;
}
#endif /* SQUIRREL */
