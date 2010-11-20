/**
 * classes.h - Class definitions for the Squirrel
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */
/*
 * classes.h
 * Common classes definition
 */
#ifndef __CLASSES_H__
#define __CLASSES_H__

enum class_id_nuts
{
    GENERIC_CLASS = 0x01,
    WEATHERSTATION = 0x02,
    TEST_NUT = 0x03
};

enum class_id_sensors
{
    GENERIC_SENSOR = 0x00,

    TEMPERATURE = 0x01,
    PRESSURE  = 0x02,
    LIGHT = 0x03,
    HUMIDITY = 0x04
};

enum class_id_actors
{
    GENERIC_ACTOR = 0x80,

    SWITCH  = 0x81,
    DIMMER  = 0x82,
    LED   = 0x83,

    INVALID  = 0xFF // Used to mark N/A
};
#endif
