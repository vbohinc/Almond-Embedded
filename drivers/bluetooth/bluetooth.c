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


typedef enum {
  BT_DATA,
  BT_CMD
} communication_mode_t;

typedef enum {
  BT_TEST,                      // AT
  BT_CONNECT,                   // ATA
  BT_DISCONNECT,                // ATH
  BT_CLEAR_ADDRESS,             // ATD0
  BT_SET_ADDRESS,               // ATD=_____
  BT_FIND_DEVICES,              // ATF?
  BT_DISABLE_AUTOCONNECT,       // ATO1
  BT_SET_MASTER,                // ATR0
  BT_SET_SLAVE                  // ATR1
} bt_cmd_t;

typedef enum {
  BT_RES_OK,
  BT_RES_ERROR,
  BT_RES_CONNECT,
  BT_RES_DISCONNECT,
  BT_RES_NONE
} bt_result_t;

// FIXME: NUT!
#ifdef SQUIRREL
  #define IN_FIFO_SIZE 255
#endif

#ifdef NUT
  #define IN_FIFO_SIZE 127
#endif

static uint8_t bt_buffer[IN_FIFO_SIZE];

static fifo_t in_fifo; 


static bt_mode_t bt_mode;

static communication_mode_t comm_mode = BT_CMD;
// Private helper


static void
uart_receive (void)
{
  unsigned int uart_data;
  
  // FIXME: Check if there is place in the buffer...
  while (!fifo_is_full(&in_fifo))
    {
      uart_data = uart_getc ();
      
      switch (uart_data & 0xFF00)
        {
          /* Framing Error detected, i.e no stop bit detected */
          case UART_FRAME_ERROR:
            warn_pgm(PSTR("FRM ERR"));
            return;
          
          /*
           * Overrun, a character already presend in the UART UDR register was
           * not read by the interrupt handler before the next character arrived,
           * one or more received characters have been dropped
           */   
          case UART_OVERRUN_ERROR:
            warn_pgm(PSTR("OVR ERR")); //Overrun Error
            return;
          
          /*
           * We are not reading the receive buffer fast enough,
           * one or more received character have been dropped
           */  
          case UART_BUFFER_OVERFLOW:
            warn_pgm(PSTR("BUF ERR")); //Buffer Overflow error
            return;
        
          case UART_NO_DATA:
            return;
            
          default:
            // FIXME: FIFO
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
  
  warn_pgm(PSTR("FIFO OVR ERR"));       
}

static void
uart_send (const char *data, const uint8_t length)
{
  for (uint8_t i = 0; i < length; i++)
    {
      while (uart_putc (data[i]) == 0)
        {
          warn_pgm (PSTR("UART: Remote not ready"));
          _delay_ms (1);
        }
    }
}

static bt_result_t
get_result (void)
{
  uart_receive();
  if (fifo_cmp_pgm (&in_fifo, PSTR("OK\r\n")))
    return BT_RES_OK;
  if (fifo_cmp_pgm (&in_fifo, PSTR("ERROR\r\n")))
    return BT_RES_ERROR;
  if (fifo_cmp_pgm (&in_fifo, PSTR("CONNECT\r\n")))
    return BT_RES_CONNECT;
  if (fifo_cmp_pgm (&in_fifo, PSTR("DISCONNECT\r\n")))
    return BT_RES_DISCONNECT;
  return BT_RES_NONE;
}

static bool
send_cmd (bt_cmd_t command, const char *data)
{
 
  //needed for set address
  char full_command[20];
  switch (command) {
    case BT_TEST:
      strcpy_P(full_command, PSTR("AT"));
      break;
  
    case BT_CONNECT:
      strcpy_P(full_command, PSTR("ATA"));
      break;
    
    case BT_DISCONNECT:
      strcpy_P(full_command, PSTR("ATH"));
      break;
      
    case BT_CLEAR_ADDRESS:
      strcpy_P(full_command, PSTR("ATD0"));
      break;
      
    case BT_SET_ADDRESS:
      strcpy_P(full_command, PSTR("ATD="));
      strcat(full_command,data);
      break;
          
    case BT_FIND_DEVICES:
      strcpy_P(full_command, PSTR("ATF?"));
      break;
    
    case BT_DISABLE_AUTOCONNECT:
      strcpy_P(full_command, PSTR("ATO1"));
      break;
      
    case BT_SET_MASTER:
      strcpy_P(full_command, PSTR("ATR0"));
      break;
          
    case BT_SET_SLAVE:
      strcpy_P(full_command, PSTR("ATR1"));
      break;
    
    default:
      return false;
  }
  strcat_P(full_command, PSTR("\r\n"));
  
  //throw away your television
  uart_receive ();
  fifo_clear (&in_fifo);
  
  //send command
  uart_send (full_command, strlen(full_command));
  
  //get response
  for (uint16_t i = 1000; i > 0; i--)
    {
      bt_result_t result = get_result();
      switch(result)
      {
        case BT_RES_NONE:
          continue;
        
        case BT_RES_ERROR:
          return false;
          
        default:
          return true;
      }
      _delay_ms (1);
    }
  warn_pgm (PSTR("CMD SEND: TIMEOUT"));
  return false;
}

static bool
check_connect(void)
{
  if(get_result() == BT_RES_CONNECT)
  {
    bt_mode = BT_DATA;
    return true;
  }
  return false;
}

static bool
check_disconnect(void)
{
  if(get_result() == BT_RES_DISCONNECT)
  {
    bt_mode = BT_CMD;
    return true;
  }
  return false;
}

static bool
check_package(const uint8_t *data, const uint8_t *length)
{
  uart_receive();
  if(!fifo_is_empty(&in_fifo))
  {
    if(check_disconnect())
      return false;

    fifo_read(&in_fifo, (char *)length);
    
    //warning!! if no data arrives this function will be stuck
    for(int i = 0; i < *length; i++)
    {
      while(fifo_is_empty(&in_fifo))
        uart_receive();
      
      if(check_disconnect())
        return false;
      
      fifo_read(&in_fifo, (char*)&data[i]);
      
    }
    return true;
  }
  return false;
}

bool
bt_init (void)
{
  uart_init (UART_BAUD_SELECT (UART_BAUD_RATE, F_CPU));
  
  //init fifos
  fifo_init (&in_fifo, bt_buffer, IN_FIFO_SIZE);
  
  bool connection = false;
  for(int i = 0; i < 5; i++)
  {
    if(send_cmd(BT_TEST, NULL))
    {
      connection = true;
      break;
    }
  }
  if(!connection)
  {
    error_pgm("BT: Init failed");
    return false;
  }
  
  if(!send_cmd(BT_CLEAR_ADDRESS, NULL))
    return false;
  
  send_cmd(BT_DISABLE_AUTOCONNECT, NULL); //fails in slave so no test
  
  return true;
}

bool
bt_set_mode (const bt_mode_t mode)
{
  if(mode == BLUETOOTH_MASTER)
  {
    if(send_cmd(BT_SET_MASTER, NULL))
    {
      bt_mode = mode;
      return true;
    }
    else
      return false;
  }
  else if (mode == BLUETOOTH_SLAVE)
  {
    if(send_cmd(BT_SET_SLAVE, NULL))
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
bt_receive (const uint8_t *data, const uint8_t *length)
{
  if(comm_mode == BT_CMD)
  {
    if(!check_connect())
    {
      return false;
    }
  }
  //comm_mode == BT_DATA 
  return check_package(data, length);
}


bool 
bt_send (const char *data, const uint8_t length)
{
  uart_send ((char) length, 1);
  uart_send (data, length);
  return check_disconnect();
}

#ifdef SQUIRREL
bool 
bt_connect (const char *address)
{
  return false;
}

bool 
bt_disconnect (void)
{
  uart_putc('+');
  for(int i = 0; i < 3; i++)
  {
    _delay_ms(1500);
    uart_putc('+');
  }
  comm_mode = BT_CMD;
  return send_cmd(BT_DISCONNECT, NULL);
}


bool
bt_discover (const char *result)
{
  return false;
}
#endif /* SQUIRREL */


