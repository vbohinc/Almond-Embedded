/**
 * RTC / timer driver for a PCF 8535 on the TWI interface
 * @file rtc.c
 * @author Linus Lotz
 */

#include <rtc/rtc.h>
#include <error.h>
#include <twi/twi_master_driver.h>
#define RTC_DEV_ADDRESS 0x51

/*! CPU speed 2MHz, BAUDRATE 100kHz and Baudrate Register Settings */
#define CPU_SPEED       2000000
#define BAUDRATE	100000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)

TWI_Master_t twiMaster;

ISR(TWIE_TWIM_vect)			// Wichtig
{
      TWI_MasterInterruptHandler(&twiMaster);
}

void rtc_init()
{
	TWI_MasterInit(&twiMaster,&TWIE,TWI_MASTER_INTLVL_HI_gc,TWI_BAUDSETTING);
	PORTC.DIR |= 0x03;
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	PMIC.CTRL |= PMIC_MEDLVLEN_bm;
	PMIC.CTRL |= PMIC_HILVLEN_bm;
}

uint8_t bcd_to_int (uint8_t bcd)
{
    return ( (bcd & 0xf0) >> 4) *10 + (bcd & 0x0f);
}

uint8_t int_to_bcd (uint8_t integer)
{
    return ( (integer / 10) << 4) + (integer % 10);
}

uint8_t get_value (uint8_t address)
{
	TWI_MasterWriteRead(&twiMaster, RTC_DEV_ADDRESS, &address, 1, 1);
	while (!TWI_MasterReady(&twiMaster));
	byte_to_hex(twiMaster.readData[0]);
    return twiMaster.readData[0];
}

void set_value (uint8_t address, uint8_t value)
{
	uint8_t sendarray[2] = {address, value};
	TWI_MasterWriteRead(&twiMaster, RTC_DEV_ADDRESS, sendarray, 2, 0);
	debug("sending");
	while (!TWI_MasterReady(&twiMaster));
}

time_t get_time()
{
    uint8_t    time[6];
    time_t     result;

    for (int i = 0; i < 5; i++)
        time[i] = get_value (i + 2);

    result.year  = get_value (0xff) << 2;

    result.sec   = bcd_to_int (time[0]);

    result.min   = bcd_to_int (time[1]);

    result.hour  = bcd_to_int (time[2] & 0x3F);

    result.mday  = bcd_to_int (time[3] & 0x3F);

    result.month = bcd_to_int (time[4] & 0x1F);

    result.wday  = (time[4] & 0xE0) >> 5;

    result.year += ( (0xB0 & time[3]) >> 6);

    return result;
}

void set_time (time_t* time)
{
    uint8_t    data[6];
    data[0]    = int_to_bcd (time->sec);
    data[1]    = int_to_bcd (time->min);
    data[2]    = int_to_bcd (time->hour);
    data[3]    = int_to_bcd (time->mday) + ( (time->year % 4) << 6);
    data[4]    = int_to_bcd (time->month) + ( (time->wday) << 5);

    for (int i = 0; i < 5; i++)
        set_value (i + 2, data[i]);

    set_value (0xff, ( (time->year - (time->year % 4)) >> 2));
}


