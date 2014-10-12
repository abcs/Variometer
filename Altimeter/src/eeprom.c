/*
 * eeprom.c
 *
 *  Created on: 2014.10.12.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "eeprom.h"

static int ee_write_descriptors()
{
	return 0;
}

int ee_write_log_rec(log_rec_t * record)
{
	int rc = 0;
/*
	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
		                       RTC_control_regs, 2,
		                       0, 0, TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
*/
	return rc;
};

int ee_read_log_rec(log_rec_t * record, uint32_t rec_pointer)
{
	return 0; //TODO: return with -1 if there is no record on the pointer address
};

uint32_t ee_get_first_free_pointer()
{
	return 0;
};

