/**
 * bmp085.c - Driver for the BMP085 temprature and pressure sensor connected via TWI
 * Part of the ALMOND Project
 *     _    _     __  __  ___  _   _ ____
 *    / \  | |   |  \/  |/ _ \| \ | |  _ \
 *   / _ \ | |   | |\/| | | | |  \| | | | |
 *  / ___ \| |___| |  | | |_| | |\  | |_| |
 * /_/   \_\_____|_|  |_|\___/|_| \_|____/
 *
 */
#include <twi/twi.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

#include "bmp085.h"
#include "error.h"


struct temprature_conversion_data
{
    uint16_t AC5;
    uint16_t AC6;
    int16_t MC;
    int16_t MD;
};

struct pressure_conversion_data
{
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    int16_t B1;
    int16_t B2;
    //int16_t MB; //not needed in any calculation so won't use it
};



//the eeprom saved conversion data

struct temprature_conversion_data tempconv EEMEM = {0, 0, 0, 0};

struct pressure_conversion_data pressconv EEMEM = {0, 0, 0, 0, 0, 0};
uint8_t have_bmp_conversion_data EEMEM = 0;
const uint8_t oversampling_setting = 0;

uint16_t get_word (const uint8_t word)
{
    twi_connect (WRITE, 0x77);
    twi_write (word);
    twi_restart();
    twi_connect (READ, 0x77);
    uint8_t temp;
    uint16_t result;
    twi_read (&temp, ACK);
    result = ( ( (uint16_t) temp) << 8);

    twi_read (&temp, NACK);
    result += temp;

    twi_stop();

    return result;
}

void start_converison (const uint8_t register_value)
{
    twi_connect (WRITE, 0x77);
    twi_write (0xF4);
    twi_write (register_value);
    twi_stop();
}

void get_conversion_data (void)
{
    {

        struct temprature_conversion_data temptable;
        //temprature
        temptable.AC5 = get_word (0xB2);
        temptable.AC6 = get_word (0xB4);
        temptable.MC  = get_word (0xBC);
        temptable.MD  = get_word (0xBE);
        eeprom_write_block (&temptable, &tempconv, sizeof (struct temprature_conversion_data));
    }

    //pressure
    {

        struct pressure_conversion_data presstable;
        presstable.AC1 = get_word (0xAA);
        presstable.AC2 = get_word (0xAC);
        presstable.AC3 = get_word (0xAE);
        presstable.AC4 = get_word (0xB0);
        presstable.B1  = get_word (0xB6);
        presstable.B2  = get_word (0xB8);
        eeprom_write_block (&presstable, &pressconv, sizeof (struct pressure_conversion_data));
    }

    //debug_pgm(PSTR("condata2"));
}

int16_t calculate_true_temprature (int32_t* B5, int16_t utemprature)
{

    struct temprature_conversion_data data;
    eeprom_read_block (&data, &tempconv, sizeof (struct temprature_conversion_data));
    int32_t X1 = ( (int32_t) (utemprature - data.AC6) * (int32_t) (data.AC5)) >> 15;
    int32_t X2 = ( (int32_t) data.MC << 11) / (X1 + data.MD);
    *B5 = X1 + X2;
    return (*B5 + 8) >> 4;
}

int32_t calculate_true_pressure (int32_t* B5, int16_t upressure)
{

    struct pressure_conversion_data data;
    eeprom_read_block (&data, &pressconv, sizeof (struct pressure_conversion_data));
    //Behold: Magic!
    int32_t B6, X1, X2, X3, B3, p;
    uint32_t B4, B7;
    B6 = *B5 - 4000;
    X1 = ( (int32_t) data.B2 * ( (B6 * B6) >> 12)) >> 11;
    X2 = (data.AC2 * B6) >> 11;
    X3 = X1 + X2;
    B3 = ( ( ( ( ( (int32_t) data.AC1) << 2) + X3) << oversampling_setting) + 2) >> 2;
    X1 = (data.AC3 * B6) >> 13;
    X2 = (data.B1 * ( (B6 * B6) >> 12)) >> 16;
    X3 = ( (X1 + X2) + 2) >> 2;
    B4 = (data.AC4 * (uint32_t) (X3 + 32768)) >> 15;
    B7 = ( (uint32_t) upressure - B3) * (50000 >> oversampling_setting);

    if (B7 < 0x80000000)
    {
        p = (B7 << 1) / B4;
    }

    else
    {
        p = (B7 / B4) << 1;
    }

    X1 = (p >> 8) * (p >> 8);

    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;
    p = p + ( (X1 + X2 + 3791) >> 4);
    return p;
}

bmp_data_t bmp085_get_data()
{
    int32_t B5;
    bmp_data_t data;
    start_converison (0x2E);
    _delay_ms (6);
    data.temprature = calculate_true_temprature (&B5, get_word (0x2E));

    start_converison (0x34);
    _delay_ms (6);
    data.pressure = calculate_true_pressure (&B5, get_word (0x34));
    return data;
}

void init_bmp085_sensor()
{
    twi_init();
    uint8_t data_availible = eeprom_read_byte (&have_bmp_conversion_data);

    if (data_availible == 0)
    {
        get_conversion_data();
        eeprom_write_byte (&have_bmp_conversion_data, 1);
    }
}


