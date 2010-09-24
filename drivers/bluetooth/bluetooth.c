#include "bluetooth.h"
#include <uart/uart.h>


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


// FIXME: NUT!
#ifdef SQUIRREL
  #define IN_FIFO_SIZE 255
  #define OUT_FIFO_SIZE 255
#endif

#ifdef NUT
  #define IN_FIFO_SIZE 127
  #define OUT_FIFO_SIZE 127
#endif

static uint8_t bt_in_buffer[IN_FIFO_SIZE];
static uint8_t bt_out_buffer[OUT_FIFO_SIZE];

static fifo_t in_fifo; 
static fifo_t out_fifo;




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
            fifo_write_c (&in_fifo, uart_data))
            
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
uart_send (void)
{
  uint8_t data;
  while (fifo_read_c (&out_fifo, &data))
  {
    uart_putc (data);
  }
}

static bool
send_cmd (bt_cmd_t command, const char *data)
{
  //write command to fifo
  fifo_clear (&out_fifo);
  switch (command) {
    case BT_TEST:
      fifo_write_pgm (&out_fifo, PSTR("AT"));
      break;
  
    case BT_CONNECT:
      fifo_write_pgm (&out_fifo, PSTR("ATA"));
      break;
    
    case BT_DISCONNECT:
      fifo_write_pgm (&out_fifo, PSTR("ATH"));
      break;
      
    case BT_CLEAR_ADDRESS:
      fifo_write_pgm (&out_fifo, PSTR("ATD0"));
      break;
      
    case BT_SET_ADDRESS:
      char full_command[17];
      strcat_P(PSTR("ATD="),data);
      fifo_write (&out_fifo, full_command);
      break;
          
    case BT_FIND_DEVICES:
      fifo_write_pgm (&out_fifo, PSTR("ATF?"));
      break;
    
    case BT_DISABLE_AUTOCONNECT:
      fifo_write_pgm (&out_fifo, PSTR("ATO1"));
      break;
      
    case BT_SET_MASTER:
      fifo_write_pgm (&out_fifo, PSTR("ATR0"));
      break;
          
    case BT_SET_SLAVE:
      fifo_write_pgm (&out_fifo, PSTR("ATR1"));
      break;
    
    default:
      return false;
  }
  fifo_write_c (&out_fifo, 13); // \r
  fifo_write_c (&out_fifo, 0);
  
  //throw away your television
  uart_receive ();
  fifo_clear (in_fifo);
  
  //send command
  uart_send ();
  
  //get response (wait?)
  for (uint16_t i = 1000; i > 0; i--)
    {
      uart_receive ();
      if (fifo_cmp_pgm (&in_fifo, PSTR("OK"))
        return true;
      if (fifo_cmp_pgm (&in_fifo, PSTR("ERROR"))
        return false;
      if (fifo_cmp_pgm (&in_fifo, PSTR("CONNECT")) && command == BT_CONNECT)
        return true;
      if (fifo_cmp_pgm (&in_fifo, PSTR("DISCONNECT")) && command == BT_DISCONNECT)
        return true;
      _delay_ms (1);
    }
  warn_pgm (PSTR("CMD SEND: TIMEOUT"));
  return false;
}

static bool
check_connect()
{
  uart_receive();
  if(fifo_cmp_pgm (&in_fifo, PSTR("CONNECT")))
  {
    bt_mode = BT_DATA;
    return true;
  }
  return false;
}

static bool
check_package(const uint8_t **data, uint8_t *length)
{
  uart_receive();
  if(!fifo_is_empty(&in_fifo)
  {
    //TODO check for DISCONNECT
    //TODO read number of bytes
    //remember to call uart_receive after every read
    *length = 
    
  }
  return false;
}

bool bt_init (void)
{
  mode = disconnect;
  uart_init (UART_BAUD_SELECT (UART_BAUD_RATE, F_CPU));
  
  //init fifos
  fifo_init (&in_fifo, &bt_in_buffer, IN_FIFO_SIZE);
  fifo_init (&out_fifo, &bt_out_buffer, OUT_FIFO_SIZE);
  
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

bool bt_set_mode (const bt_mode_t mode)
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
bt_receive (const uint8_t **data, uint8_t *length)
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
bt_send (const uint8_t **data, const uint8_t length)
{
  return false;
}

#ifdef SQUIRREL
bool 
bt_connect (char *address)
{
  return false;
}

bool bt_disconnect (void)
{
  return false;
}


bool bt_discover (char *result)
{
  return false;
}
#endif /* SQUIRREL */


