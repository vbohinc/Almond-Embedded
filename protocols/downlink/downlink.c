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

  switch (bluetooth_send_data_package_with_response (&package.opcode, &length, 1))
    {
    
    case 0:
      if ((package.opcode == (RET | opcode | flag)) && (package.id == id))
        return package.value;
      else if (package.opcode == (ERROR | flag))
        error ("NSE"); // Nut signaled error
      else
        error ("DPM"); // Downlink protocol mismatch
      break;
      
    case 1:
      error ("BTE"); // Bluetooth error
      break;
      
    case 2:
      error ("NCO"); // Not connected
      break;

    case 3:
      warn ("TMO"); // Timeout
      break;
      
    default:
      error ("URV"); // Unkown return value
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

/* FIXME: Change parameter list */
void downlink_bluetooth_callback_handler (char *data_package, const uint8_t callback_type, const uint8_t data_length)
{
  bool return_package;
  downlink_package *p;

#ifdef DEBUG
  uint8_t buffer[20];
  if (callback_type == 1) //connected
    {
      bluetooth_array_to_address((char*)data_package, (char*)buffer, 0);
      debug_pgm(PSTR("CON:"));
      for (uint8_t i=0; i<12; i++)
        error_putc(buffer[i]);
      error_putc('\n');
      //connected = 1;
    }
  else if (callback_type == 2) //disconnected
    {
      bluetooth_array_to_address((char*)data_package, (char*)buffer, 0);
      debug_pgm(PSTR("DCO:"));
      for (uint8_t i=0; i<12; i++)
        error_putc(buffer[i]);
      error_putc('\n');
      //connected = 0;
    }
  else
    {

      debug_pgm(PSTR("P REC:"));
      for (uint8_t i=0; i<data_length; i++)
        {
          byte_to_hex(data_package[i]);
          error_putc(' ');
        }
      error_putc(13);
      error_putc('P');
      byte_to_hex(data_package[0]);
#endif
      if (callback_type != 0 && data_length != DOWNLINK_PACKAGE_LENGTH)
        {
          error_putc('%');
          return;
        }

      p = (downlink_package *) (data_package);
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

      switch (bluetooth_send_data_package ((uint8_t *)p, DOWNLINK_PACKAGE_LENGTH))
	  {

	  case 0: //package successfully sent
		break;

	  case 1:
		error ("BTE"); // Bluetooth error
		break;

	  case 2:
		error ("NCO"); // Not connected
		break;

	  case 3:
			warn ("TMO"); // Timeout
			break;

	  default:
		error ("URV"); // Unkown return value
	  }
#ifdef DEBUG
    }
#endif
}
#endif
