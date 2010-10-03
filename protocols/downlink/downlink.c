/**
 * downlink.c
 *
 * Downlink protocol implementation
 */

#include "downlink.h"
#include "../../drivers/bluetooth/bluetooth.h"

#ifdef SQUIRREL

downlink_package package;

/* WARNING: Assuming layer above already connected */
uint16_t downlink_request (uint8_t opcode, uint8_t flag, uint8_t id, uint16_t value, bool *err)
{
  *err = false;
  package.opcode = opcode | flag;
  package.id = id;
  package.value = value;
  uint8_t length = DOWNLINK_PACKAGE_LENGTH;

  if (bt_send (&package, length))
    {
      // FIXME: Dauerschleife
      while (!bt_receive (&package, &length))
        _delay_ms (1);
        
      if ((package.opcode == (RET | opcode | flag)) && (package.id == id))
        return package.value;
      else if (package.opcode == (ERROR | flag))
        error ("NSE"); // Nut signaled error
      else
        error ("DPM"); // Downlink protocol mismatch
    }
          
  *err = true;
  return 0;
}

uint16_t downlink_get_sensor_value (uint8_t id, bool *err)
{
  return downlink_request (GET, STANDARD, id, 0, err);
}

void downlink_set_actuator_value (uint8_t id, uint16_t value, bool *err)
{
  downlink_request (SET, STANDARD, id, value, err);
}

uint8_t downlink_get_nut_class (bool *err)
{
  return downlink_request (GET, INFO_NUT, 0, 0, err);
}

uint8_t downlink_get_extension_class (uint8_t id, bool *err)
{
  return downlink_request (GET, INFO_EXTENSION, id, 0, err);
}

void downlink_bye (uint16_t time_sec, bool *err)
{
  downlink_request (BYE, 0, 0, time_sec, err);
}
#endif

#ifdef NUT

/**
 * Handle GET package
 */
static inline bool downlink_handle_get_package (downlink_package *p)
{
  switch (p->opcode & 0x0F)
    {

    case STANDARD:
      if (p->id < class_id_extensions_length && class_id_extensions[p->id] < GENERIC_ACTOR)
        p->value = get_value (p->id);
      else
        return false;
      break;

    case INFO_NUT:
      p->id = 0;
      p->value = class_id_nut;
      break;

    case INFO_EXTENSION:
      if (p->id >= class_id_extensions_length) return false;
      p->value = class_id_extensions[p->id];
      break;

    default:
      return false;
    }

  return true;
}

/**
 * Handle SET packages
 */
static inline bool downlink_handle_set_package (downlink_package *p)
{
  switch (p->opcode & 0x0F)
    {

    case STANDARD:
      if (p->id < class_id_extensions_length && class_id_extensions[p->id] >= GENERIC_ACTOR)
        set_value (p->id, p->value);
      else
        return false;
      break;

    default:
      return false;
    }

  return true;
}

bool downlink_process_pkg (uint8_t * data, uint8_t length)
{
  bool return_package;
  downlink_package *p;

#ifdef DEBUG
  debug_pgm (PSTR ("P LEN:"));
  
  byte_to_hex (length);
  error_putc(13);
  
  debug_pgm (PSTR ("P REC:"));
  for (uint8_t i = 0; i < length; i++)
    {
      byte_to_hex(data[i]);
      error_putc(' ');
    }
  
  error_putc(13);
#endif

  if (length != DOWNLINK_PACKAGE_LENGTH)
    return false;
    
  p = (downlink_package *) (data);
  sleep = 0;

  switch (p->opcode & 0xF0)
    {
      case GET:
        return_package = downlink_handle_get_package (p);
        break;

      case SET:
        return_package = downlink_handle_set_package (p);
        break;

      case BYE:
        sleep = p->value; 
        p->id = 0;
        p->value = 0;
        return_package = true;
        break;

      case ECHO:
        return_package = true;
        break;

      default:
        return_package = false;
        break;
    }

  p->opcode |= return_package ? RET : ERROR;
  return bt_send (p, DOWNLINK_PACKAGE_LENGTH);
}

#endif
