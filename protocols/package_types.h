/**
 * package_types.h - Package definitions
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */

#ifndef __PACKAGE_TYPES_H__
#define __PACKAGE_TYPES_H__

enum package_type
{
    ECHO  = 0x00, // Debug
    GET   = 0x10, // Used by: Squirrel/Backend
    SET   = 0x20, // Used by: Squirrel/Backend
    BYE   = 0x40, // Sends the receiving unit offline for p->value seconds. (Power Saving)
    RET   = 0x80, // Used by: Nut/Squirrel
    ERROR = 0xF0, // Signal errors
};

enum package_type_flags
{
    // Nut specific
    STANDARD = 0x00, // Standard modifier
    INFO_NUT = 0x01, // Get the class of the Nut
    INFO_EXTENSION = 0x02, // Get the class of the Sensor/Actuator

    // Squirrel specific
    LOG  = 0x0A,  // Returns the latest log data in chronological order
    TIME = 0x0B, // Access the time of the squirrel, (get, set)
    LIST = 0x0C,  // Returns a list of known devices
};

#endif
