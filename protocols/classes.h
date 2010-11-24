/**
 * @file classes.h
 * Protocol common classes definition
 * @addtogroup Protocols
 */
/**@{*/
#ifndef __CLASSES_H__
#define __CLASSES_H__

///Nuts classes enum
enum class_id_nuts
{
  GENERIC_CLASS = 0x00, ///< Generic nut class
  WEATHERSTATION = 0x01 ///< Weatherstation nut class
};

///Sensor classes enum
enum class_id_sensors
{
  GENERIC_SENSOR = 0x00,///< Generic sensor class

  TEMPERATURE = 0x01, ///< Temperature sensor class
  PRESSURE 	= 0x02, ///<pressure senso class
  LIGHT = 0x03, ///< light sensor class
  HUMIDITY = 0x04 ///< humidity sensor class
};

///Actor id classes enum
enum class_id_actors
{
  GENERIC_ACTOR = 0x80, ///< Generic actor class 

  SWITCH		= 0x81, ///< switch type actor class
  DIMMER		= 0x82, ///< dimmer actor class
  LED			= 0x83, ///< LED actor class

  INVALID		= 0xFF ///< Used to mark N/A
};
#endif
/**@}*/
